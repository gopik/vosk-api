#include "kaldi_gmm_recognizer.h"

#include "kaldi_recognizer.h"
#include "json.h"
#include "fstext/fstext-utils.h"
#include "online2/online-gmm-decoding.h"

using namespace fst;
const string EMPTY = "";

KaldiGmmRecognizer::KaldiGmmRecognizer(Model *model, float sample_frequency):
    model_(model),
    max_alternatives_(10),
    gmm_models_(model->gmm_decode_config_),
    sample_frequency_(sample_frequency) {
    
    model_->Ref();
    pipeline_.reset(new OnlineFeaturePipeline(OnlineFeaturePipelineConfig(model_->gmm_pipeline_config_)));
    decoder_.reset(new SingleUtteranceGmmDecoder(
            model_->gmm_decode_config_,
            gmm_models_,
            *pipeline_,
            *model_->hclg_fst_,
            adaptation_state_));
    state_ = RECOGNIZER_INITIALIZED;
}

KaldiGmmRecognizer::~KaldiGmmRecognizer() {
    model_->Unref();
}

bool KaldiGmmRecognizer::AcceptWaveform(const char *data, int len) {
    Vector<BaseFloat> wave;
    wave.Resize(len / 2, kUndefined);
    for (int i = 0; i < len / 2; i++)
        wave(i) = *(((short *)data) + i);
    return AcceptWaveform(wave);
}
bool KaldiGmmRecognizer::AcceptWaveform(const short *sdata, int len) {
    Vector<BaseFloat> wave;
    wave.Resize(len, kUndefined);
    for (int i = 0; i < len; i++)
        wave(i) = sdata[i];
    return AcceptWaveform(wave);
}
bool KaldiGmmRecognizer::AcceptWaveform(const float *fdata, int len) {
    Vector<BaseFloat> wave;
    wave.Resize(len, kUndefined);
    for (int i = 0; i < len; i++)
        wave(i) = fdata[i];
    return AcceptWaveform(wave);
}
bool KaldiGmmRecognizer::AcceptWaveform(Vector<BaseFloat> &wdata)
{
    // Cleanup if we finalized previous utterance or the whole feature pipeline
    if (!(state_ == RECOGNIZER_RUNNING || state_ == RECOGNIZER_INITIALIZED)) {
        CleanUp();
    }
    state_ = RECOGNIZER_RUNNING;

    int step = static_cast<int>(sample_frequency_ * 0.05);
    for (int i = 0; i < wdata.Dim(); i+= step) {
        SubVector<BaseFloat> r = wdata.Range(i, std::min(step, wdata.Dim() - i));
        decoder_->FeaturePipeline().AcceptWaveform(sample_frequency_, r);
        decoder_->AdvanceDecoding();
    }
    samples_processed_ += wdata.Dim();

    if (decoder_->EndpointDetected(model_->endpoint_config_)) {
        return true;
    }

    return false;
}

void KaldiGmmRecognizer::CleanUp() {
    if (state_ == RECOGNIZER_FINALIZED) {
        samples_processed_ = 0;
        decoder_.reset(new SingleUtteranceGmmDecoder(
                model_->gmm_decode_config_,
                OnlineGmmDecodingModels(model_->gmm_decode_config_),
                *pipeline_,
                *model_->hclg_fst_,
                adaptation_state_));
    }
}

const char * KaldiGmmRecognizer::GetResult() {
    CompactLattice clat;
    bool end_of_utterance = true;
    bool rescore_if_needed = true;
    decoder_->GetLattice(rescore_if_needed, end_of_utterance, &clat);

    if (clat.NumStates() == 0) {
        return EMPTY.c_str();
    }

    Lattice lat;
    Lattice nbest_lat;
    std::vector<Lattice> nbest_lats;

    ConvertLattice(clat, &lat);
    fst::ShortestPath(lat, &nbest_lat, max_alternatives_);
    fst::ConvertNbestToVector(nbest_lat, &nbest_lats);

    json::JSON obj;
    std::stringstream ss;
    for (int k = 0; k < nbest_lats.size(); k++) {
        Lattice nlat = nbest_lats[k];
        RmEpsilon(&nlat);
        CompactLattice nclat;
        CompactLattice aligned_nclat;
        std::vector<int32> alignment;
        std::vector<int32> words;
        LatticeWeight weight;
        GetLinearSymbolSequence(nlat, &alignment, &words, &weight);
        if (model_->word_syms_ != NULL) {
            stringstream text;
            json::JSON entry;
            for (size_t i = 0; i < words.size(); i++) {
                json::JSON word;
                std::string s = model_->word_syms_->Find(words[i]);
                if (s == "") {
                    std::cerr << "Word-id " << words[i] << " not in symbol table.";
                } else {
                    word["word"] = s;
                    entry["result"].append(word);
                }
                if (i) {
                    text << " ";
                }
                text << s;
                std::cerr << s << ' ';
            }
            entry["text"] = text.str();
            entry["confidence"] = -(weight.Value1() + weight.Value2());
            obj["alternatives"].append(entry);
        }
    }
    last_result_ = obj.dump();
    return last_result_.c_str();
}

const char * KaldiGmmRecognizer::FinalResult() {
    decoder_->FeaturePipeline().InputFinished();
    decoder_->AdvanceDecoding();
    decoder_->FinalizeDecoding();

    decoder_->EstimateFmllr(true);
    decoder_->GetAdaptationState(&adaptation_state_);
    state_ = RECOGNIZER_FINALIZED;
    GetResult();

    return last_result_.c_str();
}

const char * KaldiGmmRecognizer::PartialResult() {
    return EMPTY.c_str();
}

void KaldiGmmRecognizer::Reset() {}

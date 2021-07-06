#pragma once

#include "base/kaldi-common.h"
#include "online2/online-gmm-decoding.h"
#include "online2/online-timing.h"
#include "online2/online-endpoint.h"
#include "fstext/fstext-lib.h"

#include "kaldi_recognizer_base.h"
#include "kaldi_recognizer.h"
#include "model.h"

using namespace kaldi;

class KaldiGmmRecognizer: KaldiRecognizerBase {
    public:
        KaldiGmmRecognizer(Model *model, float sample_frequency);
        bool AcceptWaveform(const char *data, int len);
        bool AcceptWaveform(const short *sdata, int len);
        bool AcceptWaveform(const float *fdata, int len);
        bool AcceptWaveform(Vector<BaseFloat> &wdata);
        const char * Result();
        const char * FinalResult();
        const char * PartialResult();
        void Reset();
        ~KaldiGmmRecognizer();

    private:
        const char * GetResult();
        void CleanUp();
        OnlineGmmDecodingConfig decode_config_;
        OnlineGmmAdaptationState adaptation_state_;
        OnlineTimingStats timing_stats_;
        unique_ptr<OnlineFeaturePipeline> pipeline_;
        KaldiRecognizerState state_;
        unique_ptr<SingleUtteranceGmmDecoder> decoder_;
        Model* model_;

        float sample_frequency_;

        int64 samples_processed_;
        int32 max_alternatives_;
        string last_result_;
};
        

#include "kaldi_gmm_recognizer.h"
#include "model.h"
#include "online2/online-feature-pipeline.h"
#include "online2/online-gmm-decoding.h"

#include <stdio.h>
#include <ctime>

int main(int argc, char* argv[]) {
    Model* model = new Model("/root/vosk_artifacts");
    std::cout << "Created model\n";
    KaldiGmmRecognizer *rec = new KaldiGmmRecognizer(model, 16000);
    std::cout << "Created Recognizer\n";
    char buf[6400];
    FILE* wavein = fopen("test.wav", "rb");
    fseek(wavein, 44, SEEK_SET);
    
    int nread = fread(buf, 1, sizeof(buf), wavein);

    std::cout << "end" << std::endl;
    std::cout << time(NULL) << std::endl;
    while (!feof(wavein)) {
        int nread = fread(buf, 1, sizeof(buf), wavein);
        bool final = rec->AcceptWaveform(buf, nread);
        if (final) {
            std::cout << rec->FinalResult() << std::endl;
            break;
        } 
    }
    std::cout << time(NULL) << std::endl;

    std::cout << rec->FinalResult() << std::endl;
    std::cout << time(NULL) << std::endl;

    fclose(wavein);

    delete rec;
    model->Unref();
}
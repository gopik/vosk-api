#pragma once

class KaldiRecognizerBase {
    public:
	virtual bool AcceptWaveform(const char *data, int len) = 0;
	virtual bool AcceptWaveform(const short *sdata, int len) = 0;
	virtual bool AcceptWaveform(const float *fdata, int len) = 0;
	virtual const char* Result() = 0;
	virtual const char* FinalResult() = 0;
	virtual const char* PartialResult() = 0;
	virtual void Reset() = 0;
}

# About
This project is for adding gmm based recognizers to the vosk api.
Items planned:
1. Add access to online2-wav-gmm-latgen-faster.
2. Add support for regular expression based LMs. Currently we have a
   session which maintains state for ongoing recognition in streaming fashion.
   We need another abstraction where we have a connection within which sessions
   are included. We can then configure the connection with a specific grammar
   which is used to build HCLG.fst for that connection. Without this, we either
   need prebuilt HCLG.fst or build it for each session.

# About Vosk

Vosk is an offline open source speech recognition toolkit. It enables
speech recognition models for 18 languages and dialects - English, Indian
English, German, French, Spanish, Portuguese, Chinese, Russian, Turkish,
Vietnamese, Italian, Dutch, Catalan, Arabic, Greek, Farsi, Filipino,
Ukrainian.

Vosk models are small (50 Mb) but provide continuous large vocabulary
transcription, zero-latency response with streaming API, reconfigurable
vocabulary and speaker identification.

Speech recognition bindings implemented for various programming languages
like Python, Java, Node.JS, C#, C++ and others.

Vosk supplies speech recognition for chatbots, smart home appliances,
virtual assistants. It can also create subtitles for movies,
transcription for lectures and interviews.

Vosk scales from small devices like Raspberry Pi or Android smartphone to
big clusters.

# Documentation

For installation instructions, examples and documentation visit [Vosk
Website](https://alphacephei.com/vosk).

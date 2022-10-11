#ifndef FFAUDIODECODE_H_
#define FFAUDIODECODE_H_
#include "FFBaseDecoder.h"

class FFAudioDecoder: public FFBaseDecoder
{
public:
    FFAudioDecoder(const std::string& filePath);
    ~FFAudioDecoder();

private:
    bool init();
};

#endif
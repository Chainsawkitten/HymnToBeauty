#include "VorbisFile.hpp"

#include "../Util/Log.hpp"
#include <stb_vorbis.c>

using namespace Audio;

VorbisFile::VorbisFile(const char *filename) {
    int channels;
    dataSize = stb_vorbis_decode_filename(filename, &channels, &sampleRate, reinterpret_cast<short**>(&data));
    
    if (dataSize == -1)
        Log() << "Couldn't load OGG Vorbis file: " << filename << "\n";
    
    // We get size in samples, but we need it in bytes.
    dataSize *= channels * sizeof(short);
    
    if (channels > 1)
        format = AL_FORMAT_STEREO16;
    else
        format = AL_FORMAT_MONO16;
}

VorbisFile::~VorbisFile() {
    free(data);
}

const char* VorbisFile::GetData() const {
    return data;
}

ALsizei VorbisFile::GetSize() const {
    return dataSize;
}

ALenum VorbisFile::GetFormat() const {
    return format;
}

ALsizei VorbisFile::GetSampleRate() const {
    return sampleRate;
}

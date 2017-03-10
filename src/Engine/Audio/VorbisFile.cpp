#include "VorbisFile.hpp"

#include "../Util/Log.hpp"
#include <stb_vorbis.c>

using namespace Audio;

VorbisFile::VorbisFile(const char* filename) {
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
    
    // Open vorbis file.
    int error;
    stb_vorbis* vorbisFile = stb_vorbis_open_filename(filename, &error, NULL);
    if (vorbisFile == nullptr)
        Log() << "Couldn't open OGG Vorbis file: " << filename << "\n";
    
    // Show file info.
    stb_vorbis_info info = stb_vorbis_get_info(vorbisFile);
    stereo = (info.channels == 2);
    if (info.sample_rate != 44100)
        Log() << filename << ": Warning! Only sample rates of 44100 Hz supported.\n";
    
    // Get length.
    int n = 0;
    length = 0;
    do {
        int bufferSize = 256;
        float buffer[bufferSize];
        n = stb_vorbis_get_samples_float_interleaved(vorbisFile, info.channels, buffer, bufferSize);
        length += n;
    } while (n > 0);
    
    // Decode file.
    audio = new float[length * info.channels];
    stb_vorbis_seek_start(vorbisFile);
    stb_vorbis_get_samples_float_interleaved(vorbisFile, info.channels, audio, length);
    
    stb_vorbis_close(vorbisFile);
}

VorbisFile::~VorbisFile() {
    free(data);
    delete[] audio;
}

const char* VorbisFile::Data() const {
    return data;
}

ALsizei VorbisFile::Size() const {
    return dataSize;
}

ALenum VorbisFile::Format() const {
    return format;
}

ALsizei VorbisFile::SampleRate() const {
    return sampleRate;
}

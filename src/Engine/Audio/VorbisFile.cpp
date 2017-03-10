#include "VorbisFile.hpp"

#include "../Util/Log.hpp"
#include <stb_vorbis.c>
#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"

using namespace Audio;

VorbisFile::VorbisFile() {
    
}

VorbisFile::VorbisFile(const char* filename) {
    Load(filename);
}

VorbisFile::~VorbisFile() {
    delete[] audio;
}

const float* VorbisFile::GetData() const {
    return audio;
}

std::size_t VorbisFile::GetLength() const {
    return length;
}

bool VorbisFile::IsStereo() const {
    return stereo;
}

int VorbisFile::GetSampleRate() const {
    return sampleRate;
}

Json::Value VorbisFile::Save() const {
    Json::Value sound;
    sound["name"] = name;
    return sound;
}

void VorbisFile::Load(const Json::Value& node) {
    name = node.get("name", "").asString();
    Load((Hymn().GetPath() + FileSystem::DELIMITER + "Sounds" + FileSystem::DELIMITER + name + ".ogg").c_str());
}

void VorbisFile::Load(const char* filename) {
    // Open vorbis file.
    int error;
    stb_vorbis* vorbisFile = stb_vorbis_open_filename(filename, &error, NULL);
    if (vorbisFile == nullptr)
        Log() << "Couldn't open OGG Vorbis file: " << filename << "\n";
    
    // Show file info.
    stb_vorbis_info info = stb_vorbis_get_info(vorbisFile);
    stereo = (info.channels == 2);
    sampleRate = info.sample_rate;
    if (sampleRate != 44100)
        Log() << filename << ": Warning! Only sample rates of 44100 Hz supported.\n";
    
    // Get length.
    int n = 0;
    length = 0;
    do {
        const int bufferSize = 256;
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

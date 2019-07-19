#include "engine/data_file.h"
#include "pch.h"

#include <engine/mmh3.h>

namespace Data_utils {
    std::ifstream::pos_type filesize(const char* filename)
    {
        std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
        return in.tellg();
    }

    bool file_exists(const std::string& name) {
        std::ifstream f(name.c_str());
        return f.good();
    }

    uint32_t generate_hash(const char* filename) {
        uint32_t hash = 0;

        MurmurHash3_x86_32(filename, DATAFILE_FILENAME_MAX_LENGTH, 1, &hash);

        return hash;
    }

}
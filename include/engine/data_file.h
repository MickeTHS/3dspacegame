#pragma once

#include "pch.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#define DATAFILE_FILENAME_MAX_LENGTH 64

enum Data_filetype {
    MODEL = 0,
    LIGHTS,
    ANIMATION,
    DDS,
    SKELETON,
    AREA,
    SOUND,
    VEHICLE_DESCRIPTION,
    ARCHIVE,
    NONE
};

struct Data_texture_header {
    uint32_t hash;
    uint32_t datasize;
};

struct Data_texture {
    Data_texture_header header;

    uint8_t* data;

    virtual bool write(FILE* fp) {
        fwrite(&header, sizeof(Data_texture_header), 1, fp);
        fwrite(&data[0], header.datasize, 1, fp);

        return true;
    }
};

#define MDL_BONE_NAME_LENGTH 32

struct Data_bone {
    char name[MDL_BONE_NAME_LENGTH];
    int32_t parent_index;
    glm::fmat4x4 inv_bind_pose;
    glm::fmat4x4 matrix;
};

struct Data_skeleton_header {
    uint32_t num_bones;
};

struct Data_skeleton {
    Data_skeleton_header header;

    std::vector<Data_bone> bones;
};

struct Data_animation_frame {
    float time;
    glm::fvec3 translation;
    glm::fquat rotation;
};

struct Data_animation_header {
    uint32_t num_bones;
    uint32_t num_frames;
};

struct Data_animation_bone {
    uint32_t bone_index;
    uint32_t frame_offset;
    uint32_t frame_count;
};

struct Data_animation {
    Data_animation_header header;
        
    std::vector<Data_animation_bone> bones;
    std::vector<Data_animation_frame> frames;
};

struct Data_model_header {
    float bb[6];
    uint32_t rendertype;
    uint32_t diffuse_id;
    uint32_t spec_id;
    uint32_t emissive_id;
    uint32_t vertices_data_size;
    uint32_t indices_data_size;
};

struct Data_model {
    Data_model_header header;
    
    void* verticesdata;
    void* indicedata;

    Data_model() {
        
    }

    virtual bool write(FILE* fp) {
        fwrite(&header, sizeof(Data_model_header), 1, fp);
        
        fwrite(verticesdata, header.vertices_data_size, 1, fp);
        fwrite(indicedata, header.indices_data_size, 1, fp);

        return true;
    }
};

struct Data_disk_file_header {
    uint32_t    filesize;
    char        filename[DATAFILE_FILENAME_MAX_LENGTH];
    uint32_t    filetype;
};

struct Data_disk_file {
    Data_disk_file_header header;
    void*       data;

    bool write(FILE* fp, bool cast = false) {
        fwrite(&header, sizeof(Data_disk_file_header), 1, fp);

        if (!cast) {
            fwrite(data, header.filesize - sizeof(Data_disk_file_header), 1, fp);
            return true;
        }

        if (header.filetype == Data_filetype::MODEL) {
            reinterpret_cast<Data_model*>(data)->write(fp);
            return true;
        }
        else if (header.filetype == Data_filetype::DDS) {
            reinterpret_cast<Data_texture*>(data)->write(fp);
            return true;
        }
        
        return false;
    }

    bool read(FILE* fp) {
        fread(&header, sizeof(Data_disk_file_header), 1, fp);
        
        uint32_t data_size = header.filesize - sizeof(Data_disk_file_header);
        assert(data_size > 0);
        data = malloc(data_size);
        fread(data, data_size, 1, fp);

        if (header.filetype == Data_filetype::MODEL) {
            Data_model* mdl = reinterpret_cast<Data_model*>(data);
            
            return false;
        }
        else if (header.filetype == Data_filetype::DDS) {
            Data_texture* mdl = reinterpret_cast<Data_texture*>(data);

            return true;
        }
        
        return false;
    }
};

namespace Data_utils {
    bool file_exists(const std::string& name);
    std::ifstream::pos_type filesize(const char* filename);
    uint32_t generate_hash(const char* filename);
};
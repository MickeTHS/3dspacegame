#pragma once

#include "pch.h"

#include <engine/nv_dds.h>
#include <engine/gfx_shader.h>

#define TEXTURE_FILENAME_LENGTH 64

struct Gfx_texture {
	uint32_t textureId;
    char name[TEXTURE_FILENAME_LENGTH];
    
    virtual ~Gfx_texture() {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
    }

	bool load(const char* filename);
    GLuint load_dds_buffer(const uint8_t* data);
    GLuint load_dds_data(std::vector<uint8_t>& data);

	bool bind(std::shared_ptr<Gfx_shader> shader);
};

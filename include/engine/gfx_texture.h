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

// Load a .BMP file using our custom loader
GLuint loadBMP_custom(const char * imagepath);

//// Since GLFW 3, glfwLoadTexture2D() has been removed. You have to use another texture loading library, 
//// or do it yourself (just like loadBMP_custom and loadDDS)
//// Load a .TGA file using GLFW's own loader
//GLuint loadTGA_glfw(const char * imagepath);

// Load a .DDS file using GLFW's own loader
GLuint loadDDS(const char * imagepath);

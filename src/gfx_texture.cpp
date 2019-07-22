#include "pch.h"
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <engine/gfx_texture.h>
#include <engine/gfx_log.h>

bool Gfx_texture::load(const char* filename) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        printf("unknown: OpenGL error: %d\n", err);
    }

    glGenTextures(1, &textureId);

    // "Bind" the newly created texture : all future texture functions will modify this texture

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    nv_dds::CDDSImage image;
    image.load(filename);
    image.upload_texture2D();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // ... which requires mipmaps. Generate them automatically.
    glGenerateMipmap(GL_TEXTURE_2D);


    // Get a handle for our "myTextureSampler" uniform
    while ((err = glGetError()) != GL_NO_ERROR) {
        printf("Gfx_texture:load: OpenGL error: %d\n", err);
    }

    return true;
}

bool Gfx_texture::bind(std::shared_ptr<Gfx_shader> shader) {
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(shader->u_tex0, 0);

    return true;
}


#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint Gfx_texture::load_dds_buffer(const uint8_t* data) { 
    unsigned char header[124];

    /* verify the type of file */
    char filecode[4];
    memcpy(&filecode[0], &data[0], 4);

    if (strncmp(filecode, "DDS ", 4) != 0) {
        return 0;
    }

    /* get the surface desc */
    memcpy(&header, &data[4], 124);

    unsigned int height = *(unsigned int*)&(header[8]);
    unsigned int width = *(unsigned int*)&(header[12]);
    unsigned int linearSize = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC = *(unsigned int*)&(header[80]);

    const uint8_t * buffer = &(data[4 + 124]);
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    //buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
    //fread(buffer, 1, bufsize, fp);
    /* close the file pointer */
    //fclose(fp);

    unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch (fourCC)
    {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        printf("FORMAT GL_COMPRESSED_RGBA_S3TC_DXT1_EXT\n");
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        printf("FORMAT GL_COMPRESSED_RGBA_S3TC_DXT3_EXT\n");
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        printf("FORMAT GL_COMPRESSED_RGBA_S3TC_DXT5_EXT\n");
        break;
    default:
        return 0;
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
            0, size, buffer + offset);

        offset += size;
        width /= 2;
        height /= 2;

        // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
        if (width < 1) width = 1;
        if (height < 1) height = 1;

    }

    return textureID;
}

GLuint Gfx_texture::load_dds_data(std::vector<uint8_t>& data) {
    return load_dds_buffer(data.data());
}
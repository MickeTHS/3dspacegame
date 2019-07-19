#pragma once

#include "pch.h"

#ifdef _DEBUG
#define SHADER_PATH "../shaders/"
#else
#define SHADER_PATH "shaders/"
#endif

#include <GL/glew.h>
#include <glm/glm.hpp>

enum Gfx_RT_type {
    NONE_INVALID = 0,
    RT_3D_POS_NORM_UV = 1,
    RT_3D_POS_UV = 2,
    RT_3D_POS_UV_ALPHA = 3,
    NUM_TYPES
};

struct SEVBO;

struct Gfx_shader {
	int32_t programId;
	int32_t u_mvp;
	int32_t u_tex0;
    int32_t u_highlight;
    int32_t u_light0pos;
    int32_t u_model;

    glm::mat4 m4_mvp;
    glm::mat4 m4_model;
    glm::vec4 v4_highlight_color;
    glm::vec3 v3_light0pos;

    std::string name;

    virtual bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId) = 0;

    virtual bool activate() {
        // Use our shader
        glUseProgram(programId);
        glUniformMatrix4fv(u_mvp, 1, GL_FALSE, &m4_mvp[0][0]);
        glUniformMatrix4fv(u_model, 1, GL_FALSE, &m4_model[0][0]);

        glUniform4fv(u_highlight, 1, &v4_highlight_color.x);
        glUniform3fv(u_light0pos, 1, &v3_light0pos.x);

        return true;
    }
protected:
    
    bool setup(const char* name_, const char* vsfile, const char* fsfile);
    
};


struct Gfx_shader_store
{
public:
    static Gfx_shader_store& get_instance()
    {
        static Gfx_shader_store instance; // Guaranteed to be destroyed.
                              // Instantiated on first use.
        return instance;
    }

    bool init();

private:
    Gfx_shader_store() {}

    std::vector<std::shared_ptr<Gfx_shader>> _shaders;
public:
    Gfx_shader_store(Gfx_shader_store const&) = delete;
    void operator=(Gfx_shader_store const&) = delete;

    std::shared_ptr<Gfx_shader> get_shader(const std::string& name) {
        for (auto s : _shaders) {
            if (s->name == name) {
                return s;
            }
        }

        return nullptr;
    }

    void add_shader(std::shared_ptr<Gfx_shader> shader) {
        _shaders.push_back(shader);
    }
};

struct Gfx_RT_3D_pos_norm_uv : Gfx_shader {
    bool setup() {
        return Gfx_shader::setup("posnormuv", "posnormuv.vs", "posnormuv.fs");
    }

    bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId);
};


struct Gfx_RT_3D_pos_col : Gfx_shader {
    bool setup() {
        return Gfx_shader::setup("poscol", "poscol.vs", "poscol.fs");
    }

    bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId);
};


struct Gfx_RT_3D_pos_uv : Gfx_shader {
    bool setup() {
        return Gfx_shader::setup("posuv", "posuv.vs", "posuv.fs");
    }

    bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId);
};

struct Gfx_RT_3D_pos_uv_alpha : Gfx_shader {
    bool setup() {
        return Gfx_shader::setup("posuv_alpha", "AlphaTextureShader.vertexshader", "AlphaTextureShader.fragmentshader");
    }

    bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId);
};
#pragma once

#include "pch.h"

#ifdef _DEBUG
#define SHADER_PATH "../shaders/"
#else
#define SHADER_PATH "shaders/"
#endif

#include <engine/gfx_log.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


enum Gfx_RT_type {
    NONE_INVALID = 0,
    RT_3D_POS_NORM_UV = 1,
    RT_3D_POS_UV = 2,
    RT_3D_POS_UV_ALPHA = 3,
    RT_3D_POS_NORM_UV_BONE = 4,
    RT_3D_POS_UV_BB = 5,
    RT_3D_PARTICLE = 6,
    NUM_TYPES
};

struct SEVBO;

struct Gfx_shader {
	int32_t program_id;
	int32_t u_mvp;
	int32_t u_tex0;
    int32_t u_highlight;
    int32_t u_light0pos;
    int32_t u_model;
    int32_t u_model_view;
    int32_t u_proj_view;
    int32_t u_pal;
    int32_t u_proj;
    int32_t u_view;
    int32_t u_rotation;

    glm::mat4 m4_mvp;
    glm::mat4 m4_proj;
    glm::mat4 m4_proj_view;
    glm::mat4 m4_model;
    glm::mat4 m4_model_view;
    glm::mat4 m4_view;
    glm::mat4 m4_rotation;

    glm::vec4 v4_highlight_color;
    glm::vec3 v3_light0pos;
    std::vector<glm::fmat4x4> m4v_pal;

    std::string name;

    Gfx_shader() {
        m4v_pal.resize(16);
    }

    virtual bool bindParticlesVBO(std::shared_ptr<SEVBO> vbo, uint32_t buffer_id) = 0;

    virtual bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId) = 0;

    virtual bool activate() {
        
        Gfx_log::Gfx_GL_debug(Gfx_log::SHADER_ACTIVATE, "shader activate");

        std::string nn = name;

        // Use our shader
        glUseProgram(program_id);

        glUniformMatrix4fv(u_mvp, 1, GL_FALSE, &m4_mvp[0][0]);
        glUniformMatrix4fv(u_model, 1, GL_FALSE, &m4_model[0][0]);
        glUniformMatrix4fv(u_model_view, 1, GL_FALSE, &m4_model_view[0][0]);

        if (u_proj_view >= 0) {
            glUniformMatrix4fv(u_proj_view, 1, GL_FALSE, &m4_proj_view[0][0]);
        }

        if (u_proj >= 0) {
            glUniformMatrix4fv(u_proj, 1, GL_FALSE, &m4_proj[0][0]);
        }

        if (u_view >= 0) {
            glUniformMatrix4fv(u_view, 1, GL_FALSE, &m4_view[0][0]);
        }
        

        glUniformMatrix4fv(u_rotation, 1, GL_FALSE, &m4_rotation[0][0]);

        glUniform4fv(u_highlight, 1, &v4_highlight_color.x);
        glUniform3fv(u_light0pos, 1, &v3_light0pos.x);

        if (u_pal >= 0) {
            glProgramUniformMatrix4fv(program_id, u_pal, m4v_pal.size(), GL_FALSE, (GLfloat*)m4v_pal.data());
        }

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


struct Gfx_RT_3D_pos_norm_uv_bone : Gfx_shader {
    bool setup() {
        return Gfx_shader::setup("pos_norm_uv_bone", "pos_norm_uv_bone.vs", "pos_norm_uv_bone.fs");
    }

    bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId);
    bool bindParticlesVBO(std::shared_ptr<SEVBO> vbo, uint32_t buffer_id) { return true; }
};

struct Gfx_RT_3D_pos_norm_uv : Gfx_shader {
    bool setup() {
        return Gfx_shader::setup("pos_norm_uv", "pos_norm_uv.vs", "pos_norm_uv.fs");
    }

    bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId);
    bool bindParticlesVBO(std::shared_ptr<SEVBO> vbo, uint32_t buffer_id) { return true; }
};


struct Gfx_RT_3D_pos_col : Gfx_shader {
    bool setup() {
        return Gfx_shader::setup("pos_col", "pos_col.vs", "pos_col.fs");
    }

    bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId);
    bool bindParticlesVBO(std::shared_ptr<SEVBO> vbo, uint32_t buffer_id) { return true; }
};


struct Gfx_RT_3D_pos_uv : Gfx_shader {
    bool setup() {
        return Gfx_shader::setup("pos_uv", "pos_uv.vs", "pos_uv.fs");
    }

    bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId);
    bool bindParticlesVBO(std::shared_ptr<SEVBO> vbo, uint32_t buffer_id) { return true; }
};

struct Gfx_RT_3D_pos_uv_bb : Gfx_shader {
    bool setup() {
        return Gfx_shader::setup("pos_uv_bb", "pos_uv_bb.vs", "pos_uv_bb.fs");
    }

    bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId);
    bool bindParticlesVBO(std::shared_ptr<SEVBO> vbo, uint32_t buffer_id) { return true; }
};


struct Gfx_RT_3D_pos_uv_alpha : Gfx_shader {
    bool setup() {
        return Gfx_shader::setup("pos_uv_alpha", "AlphaTextureShader.vertexshader", "AlphaTextureShader.fragmentshader");
    }

    bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId);
    bool bindParticlesVBO(std::shared_ptr<SEVBO> vbo, uint32_t buffer_id) { return true; }
};


struct Gfx_RT_3D_particle : Gfx_shader {
    bool setup() {
        return Gfx_shader::setup("particle", "particle.vs", "particle.fs");
    }

    bool bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId) { return true; }
    bool bindParticlesVBO(std::shared_ptr<SEVBO> vbo, uint32_t buffer_id);
};
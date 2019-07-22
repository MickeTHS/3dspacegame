#pragma once
#include "pch.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Gfx_log {
    enum Scopes {
        WINDOW_MAIN_RENDER = 0,
        MAIN_LOOP,
        SHADER_INIT,
        SHADER_ACTIVATE,
        BIND_VBO,
        TEXTURE_ACTIVATE,
        DRAWOBJ_DRAW
    };


    struct Gfx_GL_debug {
        Gfx_GL_debug(Scopes scope, const char* msg) {
            set_debug_scope(scope, msg);
        }

        virtual ~Gfx_GL_debug() {
            unset_debug_scope();
        }

    private:

        void set_debug_scope(Scopes scope, const char* msg) {
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, static_cast<GLuint>(scope), strlen(msg), msg);
        }

        void unset_debug_scope() {
            glPopDebugGroup();
        }
    };
    


};
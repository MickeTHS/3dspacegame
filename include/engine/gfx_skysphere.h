#pragma once

#include "pch.h"
#include <glm/glm.hpp>

#include <engine/gfx_shader.h>
#include <engine/gfx_camera.h>
#include <engine/gfx_object.h>
#include <engine/gfx_texture.h>

struct Gfx_skysphere {
    Gfx_skysphere(std::shared_ptr<Gfx_camera> camera);
    virtual ~Gfx_skysphere();

    void draw(float delta);

    std::shared_ptr<Gfx_shader> _shader;
    std::shared_ptr<Gfx_camera> _camera;
    std::shared_ptr<Gfx_draw_object> _draw_obj;
    std::shared_ptr<Gfx_texture> _tex;
};
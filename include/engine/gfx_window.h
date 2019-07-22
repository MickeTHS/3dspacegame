#pragma once

#include "pch.h"

#include <glm/glm.hpp>
#include <engine/gfx_camera.h>
#include <engine/gfx_sevbo.h>

struct GLFWwindow;

struct Gfx_window {
    Gfx_window();
    virtual ~Gfx_window();

    bool init();
    bool run(std::function<void()> drawcall);
    void mouse_button_pressed(int button, int action, int mods);
    void mouse_moved(double x, double y);

    float yaw();
    float pitch();

    Gfx_camera& get_camera();

private:
    GLFWwindow* _window;
    Gfx_camera _camera;
    bool _mouse_init;
    glm::vec2 _mouse_pos;
    std::shared_ptr<Gfx_draw_object> _crosshair;
    std::shared_ptr<Gfx_shader> _pos_col;

    float _yaw;
    float _pitch;
};
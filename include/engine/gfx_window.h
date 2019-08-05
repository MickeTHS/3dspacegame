#pragma once

#include "pch.h"

#include <glm/glm.hpp>
#include <engine/gfx_camera.h>
#include <engine/gfx_sevbo.h>
#include "win32/timer.h"

struct GLFWwindow;

struct Gfx_window {
    Gfx_window();
    virtual ~Gfx_window();

    bool init(std::shared_ptr<Gfx_timer> timer);
    bool run(std::function<void()> drawcall);
    void mouse_button_pressed(int button, int action, int mods);
    void mouse_moved(double x, double y);
    void key_action(int key, int action, int mods);
    float yaw();
    float pitch();

    std::shared_ptr<Gfx_camera> get_camera();

private:
    GLFWwindow* _window;
    std::shared_ptr<Gfx_camera> _camera;
    bool _mouse_init;
    glm::vec2 _mouse_pos;
    std::shared_ptr<Gfx_draw_object> _crosshair;
    std::shared_ptr<Gfx_shader> _pos_col;

    float _yaw;
    float _pitch;

    float _width;
    float _height;

    std::shared_ptr<Gfx_timer> _timer;
};
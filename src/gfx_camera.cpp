#include "engine/gfx_camera.h"
#include <glm/gtx/transform.hpp>
#include "pch.h"


Gfx_camera::Gfx_camera() {
    _forward = false;
    _backward = false;
    _left = false;
    _right = false;


    // Initial position : on +Z
    _gg_pos = glm::vec3(0, 0, 0);
    // Initial horizontal angle : toward -Z
    _gg_horizontal_angle = 3.14f;
    // Initial vertical angle : none
    _gg_vertical_angle = 0.0f;
    // Initial Field of View
    _gg_initial_fov = 45.0f;

    _gg_speed = 3.0f; // 3 units / second
    _gg_mouse_speed = 0.005f;
}

void Gfx_camera::update_direction(float mouse_x_pos, float mouse_y_pos) {

    // Compute new orientation
    _gg_horizontal_angle += _gg_mouse_speed * mouse_x_pos;
    _gg_vertical_angle += _gg_mouse_speed * mouse_y_pos;

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
        cos(_gg_vertical_angle) * sin(_gg_horizontal_angle),
        sin(_gg_vertical_angle),
        cos(_gg_vertical_angle) * cos(_gg_horizontal_angle)
    );

    // Right vector
    _v_right = glm::vec3(
        sin(_gg_horizontal_angle - 3.14f / 2.0f),
        0,
        cos(_gg_horizontal_angle - 3.14f / 2.0f)
    );

    // Up vector
    _v_up = glm::cross(_v_right, direction);

    _dir = direction;
}

void Gfx_camera::update(float delta) {
    float deltaTime = delta;
    
    // Move forward
    if (_forward) {
        _gg_pos += _dir * deltaTime * _gg_speed;
    }
    // Move backward
    if (_backward) {
        _gg_pos -= _dir * deltaTime * _gg_speed;
    }
    // Strafe right
    if (_right) {
        _gg_pos += _v_right * deltaTime * _gg_speed;
    }
    // Strafe left
    if (_left) {
        _gg_pos -= _v_right * deltaTime * _gg_speed;
    }

    _view = glm::lookAt(
        _gg_pos,           // Camera is here
        _gg_pos + _dir, // and looks here : at the same position, plus "direction"
        _v_up                  // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void Gfx_camera::set_lookat(glm::vec3 point) {
    _view = glm::lookAt(
        _gg_pos,           // Camera is here
        point, // and looks here : at the same position, plus "direction"
        _v_up  // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void Gfx_camera::set_move_forward(bool move) {
    _forward = move;
}

void Gfx_camera::set_move_left(bool move) {
    _left = move;
}

void Gfx_camera::set_move_right(bool move) {
    _right = move;
}

void Gfx_camera::set_move_backward(bool move) {
    _backward = move;
}


void Gfx_camera::init(float width, float height) {
    //_proj = glm::perspective(glm::radians(90.0f), width / height, 0.1f, 100.0f);
    _proj = glm::perspective(glm::radians(_gg_initial_fov), width / height, 0.1f, 100.0f);
    _gg_pos = glm::vec3(0, 0, 0);
    _width = width;
    _height = height;
    _v_up = glm::vec3(0, 1, 0);
    _dir = glm::vec3(0, 0, -1);
}


glm::mat4 Gfx_camera::get_proj() {
    return _proj;
}

glm::mat4 Gfx_camera::get_view() {
    return _view;
}

glm::vec3 Gfx_camera::get_pos() {
    return _gg_pos;
}

glm::vec3 Gfx_camera::get_lookat() {
    return glm::vec3(_view[2][0], _view[2][1], _view[2][2]);
}
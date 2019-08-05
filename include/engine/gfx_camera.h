#pragma once

#include <glm/glm.hpp>

struct Gfx_camera {
    Gfx_camera();
    void init(float width, float height);

    void update(float delta);
    
    void set_move_forward(bool move);
    void set_move_left(bool move);
    void set_move_right(bool move);
    void set_move_backward(bool move);

    
    glm::mat4 get_proj();

    glm::mat4 get_view();
    glm::vec3 get_lookat();
    glm::vec3 get_pos();

    void set_lookat(glm::vec3 point);

    void update_direction(float mouse_x_pos, float mouse_y_pos);

private:
    glm::mat4 _proj;
    glm::mat4 _view;
    
    glm::vec3 _dir;

    glm::vec3 _v_right;
    glm::vec3 _v_up;

    bool _forward;
    bool _backward;
    bool _left;
    bool _right;

    float _width;
    float _height;

    glm::vec3 _gg_pos;
    float _gg_horizontal_angle;
    float _gg_vertical_angle;
    float _gg_initial_fov;
    float _gg_speed;
    float _gg_mouse_speed;
};
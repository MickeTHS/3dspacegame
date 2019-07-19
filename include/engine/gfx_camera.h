#pragma once

#include <glm/glm.hpp>

struct Gfx_camera {
    Gfx_camera();
    void init(float width, float height);

    void update();
    void set_yaw(float y);
    
    void set_pitch(float p);
    
    glm::mat4 get_proj();

    glm::mat4 get_view();
    glm::vec3 get_lookat();
    glm::vec3 get_pos();

private:
    glm::mat4 _proj;
    glm::mat4 _view;
    glm::vec3 _pos;
    glm::vec3 _lookat;

    float _pitch;
    float _yaw;
};
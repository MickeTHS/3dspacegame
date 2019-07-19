#pragma once

#include "pch.h"
#include <glm/glm.hpp>

#include <engine/gfx_sevbo.h>
#include <engine/gfx_shader.h>
#include <engine/gfx_texture.h>

enum Gfx_obj_flags {
    highlight = 0
};


struct Gfx_object {
    Gfx_object(std::shared_ptr<SEVBO> vbo, std::shared_ptr<Gfx_draw_object> object);

    std::shared_ptr<Gfx_draw_object>& get_draw_obj();

    void calc_bounds(std::vector<glm::vec3>& verts);

    void update();
    void set_pos(glm::vec3& pos);
    bool intersect(glm::vec3& orig, glm::vec3& dir);

    uint32_t get_flags();
    void set_flag(uint8_t index, bool on);
    
    glm::vec3 local_bounds[8];

private:
    uint32_t _flags;

    glm::vec3 _min;
    glm::vec3 _max;
    glm::vec3 _pos;
    glm::vec3 _world_bounds[8];
    
    glm::mat4 _mvp;

    std::shared_ptr<SEVBO> _vbo;
    std::shared_ptr<Gfx_draw_object> _draw_obj;
};
#include <engine/gfx_object.h>
#include <engine/data_file.h>
#include <glm/gtx/intersect.hpp>

Gfx_object::Gfx_object(std::shared_ptr<SEVBO> vbo, std::shared_ptr<Gfx_draw_object> object) 
: _vbo(vbo), _draw_obj(object), _animator(nullptr) {

}

void Gfx_object::set_animator(std::shared_ptr<Gfx_animator> animator) {
    _animator = animator;
}

void Gfx_object::set_skeleton(std::shared_ptr<Gfx_skeleton> skel) {
    _skeleton = skel;
}

std::shared_ptr<Gfx_draw_object>& Gfx_object::get_draw_obj() {
    return _draw_obj;
}

void Gfx_object::set_animation(const char* animation_name) {
    if (_animator == nullptr) {
        return;
    }

    auto anim = _animator->get_animation(animation_name);

    assert(anim);

    _animation_state = _animator->make_animation_state(_skeleton, animation_name);

    assert(_animation_state);
}

std::shared_ptr<Gfx_animation_state>& Gfx_object::get_animation_state() {
    return _animation_state;
}

void Gfx_object::calc_bounds(std::vector<glm::vec3>& verts) {
    local_bounds[0] = verts[0];
    local_bounds[1] = verts[1];
}

uint32_t Gfx_object::get_flags() {
    return _flags;
}

void Gfx_object::set_flag(uint8_t index, bool on) {
    if (on) {
        _flags |= 1UL << index;
    }
    else {
        _flags &= ~(1UL << index);
    }
}

void Gfx_object::update() {
    // set the world bounds
    // we assume the anchorpoint is the lower, front, left corner
    for (int i = 0; i < 8; ++i) {
        _world_bounds[i] = _pos + local_bounds[i];
    }

    // calculate the mins and max for the intersect tests
    _min.x = std::fmin(_world_bounds[0].x, _world_bounds[1].x);
    _min.y = std::fmin(_world_bounds[0].y, _world_bounds[1].y);
    _min.z = std::fmin(_world_bounds[0].z, _world_bounds[1].z);
    _max.x = std::fmax(_world_bounds[0].x, _world_bounds[1].x);
    _max.y = std::fmax(_world_bounds[0].y, _world_bounds[1].y);
    _max.z = std::fmax(_world_bounds[0].z, _world_bounds[1].z);
    
}

void Gfx_object::set_pos(glm::vec3& pos) {
    _pos = pos;
    update();
}


bool Gfx_object::intersect(glm::vec3& orig, glm::vec3& dir) {
    float tmin = (_min.x - orig.x) / dir.x;
    float tmax = (_max.x - orig.x) / dir.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (_min.y - orig.y) / dir.y;
    float tymax = (_max.y - orig.y) / dir.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (_min.z - orig.z) / dir.z;
    float tzmax = (_max.z - orig.z) / dir.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}

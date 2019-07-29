#pragma once

#include "pch.h"

#include <glm/glm.hpp>
#include "engine/gfx_skinning.h"

enum Animation_current_state {
    NOT_RUNNING = 0,
    RUNNING
};

struct Data_animation;

// the definition of an animation
struct Gfx_animation {
    char name[64];
    Data_animation* animationdata;

    Gfx_animation(const char* name_) {
        memset(name, 0, 64);
        strcpy(name, name_);
    }

};

// the state of an animation applied to an object
struct Gfx_animation_state {
    Gfx_animation_state(std::shared_ptr<Gfx_skeleton> skeleton_, std::shared_ptr<Gfx_animation> animation_) 
        : skeleton(skeleton_), animation(animation_), current_state(Animation_current_state::NOT_RUNNING)
    {}

    void start_animation(float t);

    std::vector<glm::fmat4x4> palette;
    std::shared_ptr<Gfx_animation> animation;
    std::shared_ptr<Gfx_skeleton> skeleton;

    float start_time;
    Animation_current_state current_state;

    void calc_matrix(float t, float global_time, std::vector<glm::mat4x4>& data);
};

struct Gfx_animator {
    Gfx_animator();
    virtual ~Gfx_animator();

    std::shared_ptr<Gfx_animation_state> make_animation_state(std::shared_ptr<Gfx_skeleton> skeleton, const char* animation);

    void add_animation_def(std::shared_ptr<Gfx_animation> animation);
    void add_animation(std::shared_ptr<Gfx_animation_state> animation_state);
    std::shared_ptr<Gfx_animation> get_animation(const char* animation);
    void update(float delta, std::vector<glm::mat4x4>& data);

    std::vector<std::shared_ptr<Gfx_animation_state>> _animation_states;
    std::vector<std::shared_ptr<Gfx_animation>> _animations;

private:
    float _time;
};
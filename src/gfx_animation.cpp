#include "engine/gfx_animation.h"
#include "engine/data_file.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>



void Gfx_animation_state::start_animation(float t) {
    start_time = t;
    current_state = Animation_current_state::RUNNING;
}

void Gfx_animation_state::calc_matrix(float t, float global_time, std::vector<glm::mat4x4>& data) {

    std::vector<Data_animation_bone>& anim_bones = animation->animationdata->bones;
    std::vector<Data_animation_frame>& anim_frames = animation->animationdata->frames;

    //glm::fmat4x4 tm = glm::mat4(1.0) ** frames[0].rotation

    int32_t current_frame = 0;
    int32_t next_frame = -1;

    // determine which frame we are at
    for (uint32_t i = 0; i < anim_frames.size() - 1; ++i) {
        if (anim_frames[i].time <= t && anim_frames[i + 1].time >= t) {
            current_frame = i;
            next_frame = i + 1;
        }
    }

    float lerpval = 0.0f;


    // animation is finished
    if (next_frame == -1) {
        start_animation(global_time);

        //printf("restarting animation\n");
        return;
    }

    lerpval = (t - anim_frames[current_frame].time) / (anim_frames[current_frame].time + anim_frames[next_frame].time);


    if (lerpval > 0.95f) {
        int apa = 0;


    }
    if (lerpval > 1.0f) {
        lerpval = 1.0f;
    }
    else if (lerpval == 1.0f) {
        int apa = 0;
    }

    // set the bind pose
    skeleton->calc_matrix_palette_bind(data);
    memcpy(&palette[0], &data[0], sizeof(glm::fmat4x4) * palette.size());
    lerpval = 1.0f;

    // update only the bones part of the animation... ?
    for (uint32_t i = 0; i < anim_bones.size(); ++i) {
        int32_t bi = anim_bones[i].bone_index;
        auto t1 = anim_frames[current_frame].translation;
        auto t2 = anim_frames[next_frame].translation;

        auto t3 = glm::lerp(t1, t2, lerpval);

        //printf("lerp: %f, t1: %f, %f, %f t2: %f, %f, %f, t3: %f, %f, %f\n", lerpval, t1.x, t1.y, t1.z, t2.x, t2.y, t2.z, t3.x, t3.y, t3.z);
        //printf("lerp: %f\n", lerpval);

        glm::mat4 tm = glm::translate(glm::mat4(1.0f), t3);
        tm = tm * glm::mat4(glm::slerp(anim_frames[current_frame].rotation, anim_frames[next_frame].rotation, lerpval));

        if (lerpval > 0.8f) {
            int apa = 0;
        }
        
        if (bi == 0) {
            // root bone
            palette[bi] = tm;
        }
        else {
            // absolute transform
            //palette[bi] = data[bi] * tm;
            palette[bi] = palette[skeleton->bones[bi]->parent_index] * tm;
        }
    }

    for (uint32_t i = 0; i < anim_bones.size(); ++i) {
        int32_t bi = anim_bones[i].bone_index;
        palette[bi] = palette[bi] * skeleton->bones[bi]->inv_bind_matrix;
        /*printf("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
            palette[i][0][0], palette[i][0][1], palette[i][0][2], palette[i][0][3], palette[i][1][0], palette[i][1][1], palette[i][1][2], palette[i][1][3], palette[i][2][0], palette[i][2][1], palette[i][2][2], palette[i][2][3]
        );*/
    }
}

Gfx_animator::Gfx_animator() {
    _time = 0.0f;
}

Gfx_animator::~Gfx_animator() {

}

void Gfx_animator::update(float delta, std::vector<glm::mat4x4>& data) {
    
    float t = 0.0f;

    for (auto&& a : _animation_states) {
        t = (_time - a->start_time) * 0.0001f;
        a->calc_matrix(t, _time, data);
    }

    _time += delta;

    //printf("time: %f, t: %f\n", _time, t);
}

std::shared_ptr<Gfx_animation_state> Gfx_animator::make_animation_state(std::shared_ptr<Gfx_skeleton> skeleton, const char* animation) {
    auto anim = get_animation(animation);

    assert(anim);

    std::shared_ptr<Gfx_animation_state> anim_state = std::make_shared<Gfx_animation_state>(skeleton, anim);
    anim_state->animation = anim;
    anim_state->start_time = 0.0f;
    anim_state->palette.resize(skeleton->bones.size());

    _animation_states.push_back(anim_state);

    return anim_state;
}


std::shared_ptr<Gfx_animation> Gfx_animator::get_animation(const char* animation) {
    for (auto&& a : _animations) {
        if (strcmp(animation, a->name) == 0) {
            return a;
        }
    }
    
    return nullptr;
}

void Gfx_animator::add_animation_def(std::shared_ptr<Gfx_animation> animation) {
    _animations.push_back(animation);
}


void Gfx_animator::add_animation(std::shared_ptr<Gfx_animation_state> animation_state) {
    _animation_states.push_back(animation_state);
}


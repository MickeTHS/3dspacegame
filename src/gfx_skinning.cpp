#include "engine/gfx_skinning.h"
#include "engine/data_file.h"

void Gfx_skeleton::calc_matrix_palette_bind(std::vector<glm::fmat4x4>& data) {

    for (uint32_t i = 0; i < bones.size(); ++i) {
        if (i == 0) {
            // root node
            data[i] = bones[i]->rel_matrix;
        }
        else {
            // absolute transform
            data[i] = data[bones[i]->parent_index] * bones[i]->rel_matrix;
        }
    }

    for (uint32_t i = 0; i < bones.size(); ++i) {
        data[i] = data[i] * bones[i]->inv_bind_matrix;
    }

    int apa = 0;
}


void Gfx_skeleton::calc_matrix_palette(std::vector<glm::fmat4x4>& data)
{
    /*const Skeleton::Joint* sk_joints = skeleton->joints;

    for (uint32_t i = 0; i < n_joints; ++i) {
        // local transform
        Matrix44_f tm(translation(joints[i].translation) * to_mat4(joints[i].rotation));
        if (i == 0) {
            // root node
            data[i] = tm;
        }
        else {
            // absolute transform
            data[i] = data[sk_joints[i].parent] * tm;
        }
    }
    // finalize
    for (uint32_t i = 0; i < n_joints; ++i) {
        data[i] = data[i] * sk_joints[i].inv_bind;
    }*/
}

#pragma once
#include "pch.h"

#include <glm/glm.hpp>

struct Data_bone;
struct Data_skeleton;


struct Gfx_bone {
    int32_t index;
    int32_t parent_index;
    glm::mat4 rel_matrix;
    glm::mat4 inv_bind_matrix;

    std::vector<std::shared_ptr<Gfx_bone>> children;
};

struct Gfx_skeleton {
    std::vector<std::shared_ptr<Gfx_bone>> bones;
    std::shared_ptr<Gfx_bone> root;

    void calc_matrix_palette_bind(std::vector<glm::fmat4x4>& data);
    void calc_matrix_palette(std::vector<glm::fmat4x4>& data);
};


	/* void Skeleton_state::calc_matrix_palette(Matrix44_f* data, const Skeleton* skeleton)
	{
		const Skeleton::Joint* sk_joints = skeleton->joints;

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
		}
	}


	void Skeleton_state::calc_matrix_palette_bind(Matrix44_f* data, const Skeleton* skeleton)
	{
		const Skeleton::Joint* sk_joints = skeleton->joints;
		for (uint32_t i = 0; i < n_joints; ++i) {
			if (i == 0) {
				// root node
				data[i] = sk_joints[i].rel_mat;
			}
			else {
				// absolute transform
				data[i] = data[sk_joints[i].parent] * sk_joints[i].rel_mat;
			}
		}
		for (uint32_t i = 0; i < n_joints; ++i) {
			data[i] = data[i] * sk_joints[i].inv_bind;
		}
	}


	void  Skeleton_state::calc_joint_transform(Matrix44_f& tm, const Skeleton* skeleton, uint32_t joint_index) const
	{
		uint32_t graph[32] = { joint_index };
		uint32_t levels = 1;
		
		// build graph
		while(joint_index != 0) {
			joint_index = skeleton->joints[joint_index].parent;
			graph[levels++] = joint_index;
		}

		// calc matrix
		tm = joint_transform(graph[--levels]);
		while (levels != 0) {
			tm = tm * joint_transform(graph[--levels]);
		}
    }*/
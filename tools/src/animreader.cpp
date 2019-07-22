#include "animreader.h"
#include "mdl.hpp"

#include <fstream>


#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"

bool AnimReader::read_animations(const std::string& filename, MDLPKG* pkg) {
	// open the animations file
	
	std::ifstream infile(filename);
	std::string line;

	std::vector<MDLAnimation*> animations;

	MDLAnimation* anim;
	MDLBone* bone;
	MDLAnimationFrame* frame;

	while (std::getline(infile, line)) {
		AnimParserLine parse = AnimParserLine::parse_line(line);

		if (parse.id.compare("ANIM_NAME") == 0) {
			anim = new MDLAnimation;
			anim->filename = parse.value + ".anim";
			anim->name = parse.value;
			anim->source = "DAE";
		}
		else if (parse.id.compare("KEYFRAME_TIMESTAMP") == 0) {
			frame = new MDLAnimationFrame;
			frame->time = stof(parse.value);
			frame->bone = bone;
			frame->bone_index = bone->index;
		}
		else if (parse.id.compare("NUM_BONES_IN_ANIM") == 0) {
			//anim = stoi(parse.value);
		}
		else if (parse.id.compare("BONE_NAME") == 0) {
			//anim = pkg->armature->get_joint_index(parse.value);
			bone = pkg->skeleton->get_bone(parse.value);
		}
		/*else if (parse.id.compare("BONE_LOC") == 0) {
			parse.parse_floats();

			frame->tx = parse.float_vals[0];
			frame->ty = parse.float_vals[1];
			frame->tz = parse.float_vals[2];

			frame->translation.x = frame->tx;
			frame->translation.y = frame->ty;
			frame->translation.z = frame->tz;
		}
		else if (parse.id.compare("BONE_ROT") == 0) {
			// bone rot is the last element for the joint, so we add the frame to the animation
			parse.parse_floats();

			frame->rx = parse.float_vals[0];
			frame->ry = parse.float_vals[1];
			frame->rz = parse.float_vals[2];
			frame->rw = parse.float_vals[3];

			frame->rotation.w = frame->rw;
			frame->rotation.x = frame->rx;
			frame->rotation.y = frame->ry;
			frame->rotation.z = frame->rz;
		}*/
		else if (parse.id.compare("BONE_MTX") == 0) {
			parse.parse_floats();

			//glm::mat4x4 mat(parse.float_vals);

			glm::mat4 mm;

			memcpy(&mm[0][0], parse.float_vals, sizeof(glm::mat4));
			mm = glm::transpose(mm);
			//Matrix4f mat = makeMatrix4f(parse.float_vals, _daeDoc.y_up);

			//Matrix44_f alx(mat.x);
						
			//Vector3_f pos(alx.m[3][0], alx.m[3][1], alx.m[3][2]);

			glm::vec3 pos(mm[3][0], mm[3][1], mm[3][2]);

			//Quaternion_f rot = to_quat(alx);

			glm::quat rot = glm::toQuat(mm);

			frame->translation.x = pos.x;
			frame->translation.y = pos.y;
			frame->translation.z = pos.z;
			
			frame->rotation.x = rot.x;
			frame->rotation.y = rot.y;
			frame->rotation.z = rot.z;
			frame->rotation.w = rot.w;

			frame->rx = rot.x;
			frame->ry = rot.y;
			frame->rz = rot.z;
			frame->rw = rot.w;

			frame->tx = pos.x;
			frame->ty = pos.y;
			frame->tz = pos.z;
			
			//current_frame->mtx = alx;
					
			/*current_frame->position[0] = pos[0];
			current_frame->position[1] = pos[1];
			current_frame->position[2] = pos[2];

			current_frame->rotation[0] = rot.x;
			current_frame->rotation[1] = rot.y;
			current_frame->rotation[2] = rot.z;
			current_frame->rotation[3] = rot.w;*/
		}
		else if (parse.id.compare("KEYFRAME_END") == 0) {
			anim->frames.push_back(frame);
		}
		else if (parse.id.compare("NUM_KEYFRAMES_BONE") == 0) {
			// not used
		}
		else if (parse.id.compare("ANIM_END") == 0) {
			pkg->skeleton->animations.push_back(anim);
		}
	}

	return true;
}
//
//  mdl.cpp
//  nut_to_mdl
//
//  Created by macbookpro on 2016-03-21.
//  Copyright © 2016 macbookpro. All rights reserved.
//

#include "mdl.hpp"
#include "osfuncs.h"
#include <engine/mmh3.h>
#include <sys/stat.h>
#include <cstdlib>
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"
#include "log.h"

void App::quit() {
	logmsg("application quitting, see ee.log\n");
	logclose();
	getchar();

	exit(0);
}

bool bonesort(const MDLBone* key1, const MDLBone* key2) {
	return (int)key1->index < (int)key2->index;
}

MDLTexture* MDLMaterial::get_surface_texture() {
	for (auto t : textures) {
		if (t->name.compare("lights") != 0)
			return t;
	}

	return NULL;
}

MDLTexture* MDLMaterial::get_material_propery_texture() {
	for (auto t : textures) {
		if (t->name.compare("lights") == 0)
			return t;
	}

	return NULL;
}

MDLTexture* MDLMaterial::get_diffuse_texture() {
	for (auto t : textures) {
		if (t->name.compare(t->name.length() - 2, 2, "_d") == 0)
			return t;
	}

	return NULL;
}

MDLTexture* MDLMaterial::get_lights_texture() {
	for (auto t : textures) {
		if (t->name.compare(t->name.length() - 2, 2, "_l") == 0)
			return t;
	}

	return NULL;
}

MDLTexture* MDLMaterial::get_property_texture() {
	for (auto t : textures) {
		if (t->name.compare(t->name.length() - 2, 2, "_m") == 0)
			return t;
	}

	return NULL;
}


MDLBone::MDLBone() {
	parsed = false;
}

MDLTexture::MDLTexture() {
	skip = false;
}

MDLLightGroup* MDLLightFile::get_lightgroup (const std::string& name) {
    for (auto lg : groups) {
        if (lg->name.compare(name) == 0)
            return lg;
    }
    
    return NULL;
}

MDLLight* MDLLightFile::get_light (const std::string& name) {
    for (auto g : groups) {
        for (auto l : g->lights) {
            if (l->raw_name.compare(name) == 0) {
                return l;
            }
        }
    }
    
    return NULL;
}

MDLMesh::MDLMesh () {
    tx = 0;
    ty = 0;
    tz = 0;
    rx = 0;
    ry = 0;
    rz = 0;
    rw = 0;

	material_flags = 0;
    
    vbo_size = 0;
    indice_size = 0;

	is_billboard = false;
}

bool MDLMesh::write_tex_hash () {
    for (auto m : materials) {
        if (m->type == MDLMaterialType::MDL_MTL_AV_MODEL || m->type == MDLMaterialType::MDL_MTL_AV_CS)
            return false;
    }
    
    return true;
}

std::vector<float> MDLMesh::get_bounding_box () {
    float mx = vertices[0].x, my = vertices[0].y, mz = vertices[0].z, lx = vertices[0].x, ly = vertices[0].y, lz = vertices[0].z;
    
    for (auto v : vertices) {
        if (v.x > mx) mx = v.x;
        if (v.y > my) my = v.y;
        if (v.z > mz) mz = v.z;
        if (v.x < lx) lx = v.x;
        if (v.y < ly) ly = v.y;
        if (v.z < lz) lz = v.z;
    }
    
    std::vector<float> bb;
    
    bb.push_back(lx);
    bb.push_back(ly);
    bb.push_back(lz);
    
    bb.push_back(mx);
    bb.push_back(my);
    bb.push_back(mz);
    
    return bb;
}

void MDLMesh::print () {
    std::cout << "    submesh: " << name << std::endl;
    std::cout << "    translation: " << tx << " " << ty << " " << tz << std::endl;
    std::cout << "    vertices: " << vertices.size() << std::endl;
    std::cout << "    materials: " << std::endl;
    
    for (auto m : materials) {
        std::cout << "      material: " << m->name << ", type: " << m->type << std::endl;
    }
    
}

std::vector<float> MDLModel::get_bounding_box () {
    std::vector<float> bb_submesh = submeshes[0]->get_bounding_box();
    
    for (auto s : submeshes) {
        std::vector<float> bb = s->get_bounding_box();
        
        if (bb[0] < bb_submesh[0]) bb_submesh[0] = bb[0];
        if (bb[1] < bb_submesh[1]) bb_submesh[1] = bb[1];
        if (bb[2] < bb_submesh[2]) bb_submesh[2] = bb[2];
        if (bb[3] > bb_submesh[3]) bb_submesh[3] = bb[3];
        if (bb[4] > bb_submesh[4]) bb_submesh[4] = bb[4];
        if (bb[5] > bb_submesh[5]) bb_submesh[5] = bb[5];
    }
    
    return bb_submesh;
}

void MDLModel::print () {
    std::cout << "  model: " << name << std::endl;
    
    for (auto m : submeshes) {
        m->print();
    }
}

std::vector<MDLAnimationFrame*> MDLAnimation::get_frames_for_bone (int bone_index) {
    std::vector<MDLAnimationFrame*> vf;
    
    for(auto f : frames) {
        if (f->bone_index == bone_index) {
            vf.push_back(f);
        }
    }
    
    return vf;
}

void MDLAnimation::erase_frames_for_bone(int i) {
	std::vector<MDLAnimationFrame*> tmp_frames;
	
	for (auto f : frames) {
		if (f->bone_index != i) {
			tmp_frames.push_back(f);
		}
	}

	frames = tmp_frames;
}

std::vector<int> MDLAnimation::get_bones () {
    std::vector<int> unique;
    
    for (auto f : frames) {
        bool found = false;
        
        for (auto b : unique) {
            if (b == f->bone_index) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            unique.push_back(f->bone_index);
        }
    }
    
    return unique;
}

MDLAnimationFrame* MDLAnimation::get_frame_for_time_bone (int time, int bone_index) {
    for (auto f : frames) {
        if (f->bone_index == bone_index && f->time == time) {
            return f;
        }
    }
    
    return NULL;
}

void MDLAnimation::print () {
    std::cout << "parsed animation: " << name << std::endl;
    
    for(auto f : frames) {
        std::cout << " bone: " << f->bone_index << " time: " << f->time << std::endl;
        std::cout << "  translation: " << f->tx << " " << f->ty << " " << f->tz << std::endl;
        std::cout << "  rotation: " << f->rx << " " << f->ry << " " << f->rz << " " << f->rw << std::endl;
    }
}

MDLSkeleton::MDLSkeleton () {
    num_bones = 0;
	parse_count = 0;
}

bool MDLSkeleton::cleanup() {
	for (int i = 0; i < bones.size(); ++i) {
		if (!bones[i]->parsed) {
			bones.erase(bones.begin() + i);
			return true;
		}
	}

	return false;
}

bool MDLSkeleton::add_bone (MDLBone* bone) {
   
	if (get_bone(bone->name) != NULL)
		return false;
	
	bones.push_back(bone);
    num_bones++;

	return true;
}

glm::mat4x4 MDLSkeleton::get_parent_abs_mat (const std::string& bone_name) {
    MDLBone* child = NULL;
    
    for (auto b : bones) {
        if (bone_name.compare(b->name) == 0) {
            child = b;
            break;
        }
    }
    
    if (child == NULL) {
        std::cout << "ERROR: Unable to find bone " << bone_name << std::endl;
		App::quit();
    }
    
    if (child->parent_index == -1)
        return matrix; //return the skeletons matrix
    
    for (auto b : bones) {
        if (b->index == child->parent_index) {
            return b->abs_matrix;
        }
    }
    
    std::cout << "ERROR: Unable to find parent abs matrix " << bone_name << std::endl;
    
	App::quit();
}

MDLBone* MDLSkeleton::get_bone (const std::string& name) {
    for (auto b : bones) {
        if (b->name.compare(name) == 0) {
            return b;
        }
    }
    
    return NULL;
}

MDLBone* MDLSkeleton::get_bone_by_file_index(int index) {
	for (auto b : bones) {
		if (b->file_index == index) {
			return b;
		}
	}

	return NULL;
}

MDLBone* MDLSkeleton::get_bone_by_index (int index) {
    for (auto b : bones) {
        if (b->index == index) {
            return b;
        }
    }
    
    return NULL;
}

void MDLSkeleton::print_animations () {
    if (animations.size() == 0) {
        std::cout << "NOTICE: no animations" << std::endl;
        
        return;
    }
    
    for (auto a : animations) {
        a->print();
    }
}

void MDLSkeleton::print_skeleton () {
    
    std::cout << "parsed skeleton: " << name << ", num bones: " << bones.size() << std::endl;
    
    std::cout << " translation: " << tx << " " << ty << " " << tz << std::endl;
    
    for (auto b : bones) {
        std::cout << " bone: " << b->name << ", index: " << b->index << ", parent index: " << b->parent_index << std::endl;
        std::cout << "  translation: " << b->tx << " " << b->ty << " " << b->tz << std::endl;
        std::cout << "  rotation: " << b->rx << " " << b->ry << " " << b->rz << " " << b->rw << std::endl;
    }
}

MDLPKG::MDLPKG () {
    skeleton = NULL;
	lightfile = NULL;
}

MDLMaterial* MDLPKG::get_material (const std::string& name) {
    for (auto m : materials) {
        if (m->name.compare(name) == 0) {
            return m;
        }
    }
    
    return NULL;
}

MDLTexture* MDLPKG::get_texture (const std::string& name) {
    for (auto t : textures) {
        if (t->name.compare(name) == 0)
            return t;
    }
    
    return NULL;
}

MDLMesh* MDLPKG::get_mesh_raw_name (const std::string& raw_name) {
    for (auto m : models) {
        for (auto ms : m->submeshes) {
            if (ms->raw_name.compare(raw_name) == 0) {
                return ms;
            }
        }
    }
    
    return NULL;
}

MDLModel* MDLPKG::get_model (std::string &prefix) {
    for (auto m : models) {
        if (m->name.compare(prefix) == 0) {
            return m;
        }
    }
    
    return NULL;
}

std::vector<std::string> MDLPKG::get_values (std::string& str) {
    std::istringstream iss(str);
    
    std::vector<std::string> tokens;
    copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         back_inserter(tokens));
    
    return tokens;
    
}

bool MDLPKG::parse_geometry(const pugi::xml_document& doc, const std::string& mdl_prefix, bool cast_shadow) {
    pugi::xml_node root = doc.child("nut");
    pugi::xml_node geoms = root.child("geometries");
    
	for (auto geom : geoms.children()) {
		std::string name = geom.attribute("name").value();

		printf("mesh: %s...\n", name.c_str());

		bool is_billboard = false;

		std::string prefix;
		std::string submesh;

		size_t dot = name.find(".");

		if (dot != std::string::npos) {
			prefix = name.substr(0, dot);
			submesh = name.substr(dot + 1);
		}
		else {
			prefix = name;
			submesh = name;
		}

		prefix = prefix;

		MDLModel* mdl = get_model(prefix);

		if (mdl == NULL) {
			mdl = new MDLModel;
			mdl->properties = 0;

			if (cast_shadow) {
				mdl->properties |= MDLFlag::SHADOW;
			}

			mdl->name = prefix;
			mdl->filename = mdl_prefix + mdl->name + ".mdl";
			models.push_back(mdl);
		}

		std::string verts = geom.child("vertices").text().as_string();
		std::vector<std::string> vertices = get_values(verts);

		std::string uv_string = geom.child("uv").text().as_string();
		std::vector<std::string> uvs = get_values(uv_string);

		std::string norms = geom.child("normals").text().as_string();
		std::vector<std::string> normals = get_values(norms);

		std::vector<std::string> local_bone_lookups;

		std::string indice_string = geom.child("indices").text().as_string();

		for (int i = 0; i < indice_string.length(); i++) {
			if (indice_string[i] == ',')
				indice_string[i] = ' ';
		}

		std::vector<std::string> indices = get_values(indice_string);

		// if there are no indices, then its possibly billboards
		if (indices.size() == 0) {
			if (vertices.size() <= 0) {
				std::cout << "ERROR: vertices in BILLBOARD mesh '" << name << "'" << std::endl;
				App::quit();
			}

			is_billboard = true;
		}
		else {
			if (normals.size() <= 0) {
				std::cout << "ERROR: no normals in mesh '" << name << "'" << std::endl;
				App::quit();
			}

			if (vertices.size() <= 0) {
				std::cout << "ERROR: no vertices in mesh '" << name << "'" << std::endl;
				App::quit();
			}

			if (uvs.size() <= 0) {
				std::cout << "ERROR: no UVs in mesh '" << name << "'" << std::endl;
				App::quit();
			}
		}
        
        
        
        std::string bones_string = geom.child("bones").text().as_string();
        
        for (int i = 0; i < bones_string.length(); i++) {
            if (bones_string[i] == ',')
                bones_string[i] = ' ';
        }
        
        std::vector<std::string> bones_weight = get_values(bones_string);
        
        if (bones_weight.size() > 0) {
            pugi::xml_node bg = geom.child("groups");
            
            for (auto g : bg.children()) {
				local_bone_lookups.push_back(g.attribute("name").value());
            }
        }
        
        MDLMesh* mesh = new MDLMesh;
        
		if (is_billboard) {
			mesh->is_billboard = true;

			for (int i = 0, u = 0; i < vertices.size(); i += 3, u += 2) {
				MDLVertex vert;
				vert.x = atof(vertices[i].c_str());
				vert.y = atof(vertices[i + 1].c_str());
				vert.z = atof(vertices[i + 2].c_str());

				mesh->vertices.push_back(vert);
			}
		}
		else {
			for (auto i : indices) {
				mesh->indices.push_back(atoi(i.c_str()));
			}

			for (int i = 0, u = 0; i < vertices.size(); i += 3, u += 2) {
				MDLVertex vert;
				vert.x = atof(vertices[i].c_str());
				vert.y = atof(vertices[i + 1].c_str());
				vert.z = atof(vertices[i + 2].c_str());

				vert.nx = atof(normals[i].c_str());
				vert.ny = atof(normals[i + 1].c_str());
				vert.nz = atof(normals[i + 2].c_str());

				vert.u = atof(uvs[u].c_str());
				vert.v = atof(uvs[u + 1].c_str());

				if (skeleton != NULL) {
					vert.bones = true;
					//vert.b = -1;
					//vert.bone = skeleton->get_bone_by_file_index(atoi(bones_weight[u].c_str()));
					//vert.bone = NULL;
					vert.bone_name = local_bone_lookups[atoi(bones_weight[u].c_str())];

					vert.bone = skeleton->get_bone(vert.bone_name);

					vert.w = atof(bones_weight[u + 1].c_str());

					if (vert.bone == NULL) {
						logmsg("ERROR: Bone lookup not found\n");
						App::quit();
					}
				}
				else {
					vert.bones = false;
				}

				mesh->vertices.push_back(vert);
			}
		}
        
        
        mesh->raw_name = name;
        mesh->name = name;
        
        unsigned int vertex_size = sizeof(float) * 3;
        vertex_size += sizeof(float) * 3;
        vertex_size += sizeof(float) * 2;
        
        if (skeleton != NULL) {
            vertex_size += sizeof(unsigned int);
        }
        
        mesh->vbo_size = vertex_size * (unsigned int)mesh->vertices.size();
        mesh->indice_size = (unsigned int)mesh->indices.size() * sizeof(uint32_t);
        
        mdl->submeshes.push_back(mesh);
    }
    return false;
}

/*
 <skeletons>
 <skeleton name="Armature">
 <bone name="bone1">
 <translation x="0.000000" y="0.000000" z="0.000000" />
 <rotation x="0.707107" y="0.000000" z="0.000000" w="0.707107" />
 <bone name="bone2">
 <translation x="0.000000" y="0.000000" z="1.000000" />
 <rotation x="-0.067996" y="0.929381" z="0.290169" w="0.217785" />
 <bone name="bone3">
 <translation x="-0.565730" y="-1.140820" z="1.840482" />
 <rotation x="-0.011751" y="-0.770642" z="-0.611399" w="0.179342" />
 </bone>
 </bone>
 </bone>
 </skeleton>
 </skeletons>
 */
bool MDLPKG::parse_skeleton(const std::string& prefix, const pugi::xml_document& doc) {
    pugi::xml_node root = doc.child("nut");
    
    pugi::xml_node skeletons = root.child("skeletons");
    
    if (skeletons.empty())
        return true;
    
    pugi::xml_node n_skeleton = skeletons.child("skeleton");
    
    if (n_skeleton.empty())
        return true;
    
	skeleton = new MDLSkeleton;
	skeleton->name = n_skeleton.attribute("name").value();
    skeleton->raw_name = skeleton->name;
	skeleton->filename = prefix + skeleton->name + ".skel";
    
	for (auto b : skeleton->bones) {
		b->index = -555;
	}

    parse_bones(n_skeleton, NULL);
    
	while (skeleton->cleanup());
	
	std::sort(skeleton->bones.begin(), skeleton->bones.end(), bonesort);

	skeleton->num_bones = skeleton->bones.size();

	for (auto b : skeleton->bones) {
		if (b->index == -555) {
			logmsg("strange bone\n");
		}
	}
	

    return true;
}

bool MDLPKG::parse_bones (const pugi::xml_node& bone, MDLBone* parent) {
    for (auto c : bone.children()) {
        
        if (strcmp(c.name(), "bone") != 0)
            continue;
        
		std::string name = c.attribute("name").value();

		//MDLBone* bs = skeleton->get_bone(name);
		MDLBone* bs = new MDLBone;
		bs->name = name;

		if (bs == NULL)
			continue;

		bs->index = skeleton->parse_count++;
		bs->parsed = true;

        if (parent == NULL)
            bs->parent_index = -1;
        else
            bs->parent_index = parent->index;
        
        pugi::xml_node trans = c.child("translation");
        pugi::xml_node rot = c.child("rotation");
        
        bs->tx = atof(trans.attribute("x").value());
        bs->ty = atof(trans.attribute("y").value());
        bs->tz = atof(trans.attribute("z").value());
        
		bs->translation = glm::vec3(bs->tx, bs->ty, bs->tz);

        bs->rx = atof(rot.attribute("x").value());
        bs->ry = atof(rot.attribute("y").value());
        bs->rz = atof(rot.attribute("z").value());
        bs->rw = atof(rot.attribute("w").value());
        
		bs->rotation = glm::quat(bs->rw, bs->rx, bs->ry, bs->rz);

		skeleton->bones.push_back(bs);

        parse_bones(c, bs);
    }
    
    return true;
}

/*
 <animations>
 <animation name="firstanimation" skeleton="Armature">
 <group name="bone3">
 <keyframes type="translation">
 <keyframe time="0.000000" x="0.000000" y="0.000000" z="0.000000" />
 <keyframe time="60.000000" x="0.000000" y="0.000000" z="0.000000" />
 </keyframes>
 <keyframes type="rotation">
 <keyframe time="0.000000" x="0.000000" y="0.000000" z="0.000000" w="1.000000" />
 <keyframe time="60.000000" x="0.703648" y="0.046897" z="-0.051764" w="0.707107" />
 </keyframes>
 </group>
 </animation>
 <animation name="secondanim" skeleton="Armature">
 <group name="bone3">
 <keyframes type="translation">
 <keyframe time="0.000000" x="0.000000" y="0.000000" z="0.000000" />
 <keyframe time="60.000000" x="0.000000" y="0.000000" z="0.000000" />
 </keyframes>
 <keyframes type="rotation">
 <keyframe time="0.000000" x="0.000000" y="0.000000" z="0.000000" w="1.000000" />
 <keyframe time="60.000000" x="0.703648" y="0.046897" z="-0.051764" w="0.707107" />
 </keyframes>
 </group>
 </animation>
 <animation name="secondanimation">
 </animation>
 </animations>
 */
bool MDLPKG::parse_animations (const std::string& prefix, const pugi::xml_document& doc) {
    pugi::xml_node root = doc.child("nut");
    
    pugi::xml_node animations = root.child("animations");
    
    if (animations.empty())
        return true;
    
    for (auto a : animations.children()) {
        std::string name = a.attribute("name").value();
        
        if (a.attribute("skeleton").empty()) {
            std::cout << "NOTICE: animation '" << name << "' has no skeleton attached, skipping" << std::endl;
            continue;
        }
        
        MDLAnimation* anim = new MDLAnimation;
        anim->name = name;
		anim->filename = prefix + name + ".anim";
        
        //parse each child group (bones)
        for (auto b : a.children()) {
            
            MDLBone* bone = skeleton->get_bone(b.attribute("name").value());
			
            for (auto kt : b.children()) {
                if (strcmp(kt.attribute("type").value(), "translation") == 0) {
                    
                    for (auto kf : kt.children()) {
                        int time = atoi(kf.attribute("time").value());
                        
                        MDLAnimationFrame* frame = anim->get_frame_for_time_bone(time, bone->index);
                        
                        if (frame == NULL) {
                            frame = new MDLAnimationFrame;
							frame->bone = bone;
                            frame->bone_index = bone->index;
							
                            frame->time = time;
                            
                            anim->frames.push_back(frame);
                        }
                        
                        frame->tx = atof(kf.attribute("x").value());
                        frame->ty = atof(kf.attribute("y").value());
                        frame->tz = atof(kf.attribute("z").value());

						// this is the absolute translation
						frame->translation = glm::vec3(frame->tx, frame->ty, frame->tz);
						
                    }
                }
                if (strcmp(kt.attribute("type").value(), "rotation") == 0) {
                    
                    for (auto kf : kt.children()) {
                        int time = atoi(kf.attribute("time").value());
                        
                        MDLAnimationFrame* frame = anim->get_frame_for_time_bone(time, bone->index);
                        
                        if (frame == NULL) {
                            frame = new MDLAnimationFrame;
                            
                            frame->bone_index = bone->index;
                            frame->time = time;
                            
                            anim->frames.push_back(frame);
                        }
                        
                        
                        frame->rx = atof(kf.attribute("x").value());
                        frame->ry = atof(kf.attribute("y").value());
                        frame->rz = atof(kf.attribute("z").value());
                        frame->rw = atof(kf.attribute("w").value());

						// this is the absolute rotation
						frame->rotation = glm::quat(frame->rw, frame->rx, frame->ry, frame->rz);
                    }
                }
            }
        }
        
		cleanup_animation(anim);

        skeleton->animations.push_back(anim);
    }
    
    return false;
}

void MDLPKG::cleanup_animation(MDLAnimation* animation) {
	std::vector<int> bones = animation->get_bones();

	for (auto i : bones) {
		if (animation->get_frames_for_bone(i).size() == 1)
			animation->erase_frames_for_bone(i);
	}
}

bool MDLPKG::parse_materials (const pugi::xml_document& doc) {
    pugi::xml_node root = doc.child("nut");
    pugi::xml_node mats = root.child("materials");
    
    for (auto m : mats.children()) {
        MDLMaterial* newmat = new MDLMaterial;
        
        newmat->name = m.attribute("name").value();
        newmat->shininess = 0;
		newmat->properties = 0;
		newmat->alpha = 0;
		newmat->zero_hash = false;
		newmat->zbias = 0;
		newmat->is_billboard = false;

        if (newmat->name.compare("aircraft") == 0 || newmat->name.find("vehicle") != std::string::npos) {
            newmat->type = MDLMaterialType::MDL_MTL_AV_MODEL;
			newmat->zero_hash = true;
        }
		else if (newmat->name.compare("fans") != std::string::npos) {
			newmat->type = MDLMaterialType::MDL_MTL_AV_MODEL;
		}
        else if (newmat->name.find("callsign") != std::string::npos) {
            newmat->type = MDLMaterialType::MDL_MTL_AV_CS;
        }
        else if (newmat->name.find("building") != std::string::npos) {
            newmat->type = MDLMaterialType::MDL_MTL_BUILDING;
        }
		else if (newmat->name.find("decal") != std::string::npos) {
            newmat->type = MDLMaterialType::MDL_MTL_STATIC_DECAL;
        }
		else if (newmat->name.find("general_") != std::string::npos) {
			newmat->type = MDLMaterialType::MDL_MTL_GENERAL;
		}
		else if (newmat->name.find("billboard") != std::string::npos) {
			newmat->type = MDLMaterialType::MDL_MTL_TREES;
			newmat->is_billboard = true;
		}
		else if (newmat->name.find("bird") != std::string::npos) {
			newmat->type = MDLMaterialType::MDL_MTL_AV_MODEL;
		}
        else {
            newmat->type = MDLMaterialType::MDL_MTL_SIMPLE;
        }
        
        for (auto i : m.children()) {
			if (strcmp(i.name(), "alpha_test") == 0) {
				std::string val = i.text().as_string();

				if (val.compare("True") == 0) {
					newmat->properties |= MDLMatFlag::ALPHA_TEST;
					newmat->properties |= MDLMatFlag::NO_CULLING;
				}
			}

			if (strcmp(i.name(), "alpha_blended") == 0) {
				std::string val = i.text().as_string();

				if (val.compare("True") == 0) {
					newmat->properties |= MDLMatFlag::ALPHA_BLEND;
				}
			}


            if (strcmp(i.name(), "shininess") == 0) {
                newmat->shininess = atof(i.text().as_string());
            }
           
			if (strcmp(i.name(), "zbias") == 0) {
				newmat->zbias = atof(i.text().as_string());
			}


            if (strcmp(i.name(), "texture") == 0) {
                std::string tex_name = std::string(i.attribute("name").value());
				MDLTexture* tex = get_texture(tex_name);
				
				if (tex == NULL) {
					continue;
				}
				
				tex->skip = false;

				if (newmat->type == MDLMaterialType::MDL_MTL_AV_MODEL) {
					newmat->textures.push_back(tex);

					if (tex_name.compare("lights") == 0) {
					
					}
				}
				else {
					newmat->textures.push_back(tex);
				}
            }
        }
        
        MurmurHash3_x86_32(MDLTYPE_NAMES[newmat->type], (int)strlen(MDLTYPE_NAMES[newmat->type]), 0, &newmat->hash);
        
        materials.push_back(newmat);
    }
    
    return false;
}

bool MDLPKG::parse_lights (const std::string& prefix, const pugi::xml_document& doc, float default_lightsize) {
    pugi::xml_node root = doc.child("nut");
    pugi::xml_node li = root.child("lights");
    
    if (li.empty())
        return true;
    
    lightfile = new MDLLightFile;
    lightfile->filename =  prefix + ".lights";
    
    for (auto l : li.children()) {
        MDLLight* light = new MDLLight;
		light->emit = MDLLightEmit::EMITNO;
		light->halo = MDLLightHalo::HALONO;
		light->direction.x = 0;
		light->direction.y = 0;
		light->direction.z = 0;
		light->lamp_size = default_lightsize;

        std::string name = l.attribute("name").value();
        
        light->raw_name = name;

        std::string group_name;
        std::string lamp_name;
        
		std::cout << "light: " << name << std::endl;

        if (name.substr(0,2).compare("p.") == 0) {
            light->type = MDLLightType::MDL_LIGHT_POINTSPRITE;
            name = name.substr(2);
        }
        else {
            if (strcmp(l.attribute("type").value(), "directional") == 0) {
                light->type = MDLLightType::MDL_LIGHT_SPOT;
				std::cout << "directional" << std::endl;
            }
            if (strcmp(l.attribute("type").value(), "point") == 0) {
                light->type = MDLLightType::MDL_LIGHT_LAMP;
				std::cout << "point" << std::endl;
            }
        }
        
        for (auto p : l.children()) {
            if (strcmp(p.name(), "colour") == 0) {
                light->r = atof(p.attribute("r").value());
                light->g = atof(p.attribute("g").value());
                light->b = atof(p.attribute("b").value());

				std::cout << "color: " << light->r << " " << light->g << " " << light->b << std::endl;
            }
            if (strcmp(p.name(), "attenuation") == 0) {
                light->att_l = atof(p.attribute("l").value());
                light->att_q = atof(p.attribute("q").value());
            }

			if (strcmp(p.name(), "halo") == 0) {
				std::string halo = p.text().as_string();

				if (halo.compare("YES") == 0) {
					light->halo = MDLLightHalo::HALOYES;
					std::cout << "HALO YES" << std::endl;
				}
				else {
					light->halo = MDLLightHalo::HALONO;
					std::cout << "HALO NO" << std::endl;
				}
			}

			if (strcmp(p.name(), "power") == 0) {
				light->power = atof(p.text().as_string());

				std::cout << "power: " << light->power << std::endl;
			}

			if (strcmp(p.name(), "emit") == 0) {
				std::string emit = p.text().as_string();

				if (emit.compare("YES") == 0) {
					light->emit = MDLLightEmit::EMITYES;
					std::cout << "EMIT YES" << std::endl;
				}
				else {
					light->emit = MDLLightEmit::EMITNO;
					std::cout << "EMIT NO" << std::endl;
				}
			}

			if (strcmp(p.name(), "shape") == 0) {
				float angle, blend;

				angle = atof(p.attribute("degrees").value());
				blend = atof(p.attribute("blend").value());

				light->spot_inner_angle = angle * (1.0f - blend);
				light->spot_outer_angle = angle;


				std::cout << "inner angle: " << light->spot_inner_angle << std::endl;
				std::cout << "outer angle: " << light->spot_outer_angle << std::endl;

				//light->shape_degrees = ;
				//light->shape_blend = ;
			}
			
			if (strcmp(p.name(), "size") == 0) {
				light->lamp_size = atof(p.attribute("r").value());
				std::cout << "size: " << light->lamp_size << std::endl;
			}
			if (strcmp(p.name(), "offset") == 0) {
				light->spot_offset = atof(p.attribute("value").value());
				std::cout << "offset: " << light->spot_offset << std::endl;
			}

			if (strcmp(p.name(), "distance") == 0) {
				light->distance = atof(p.attribute("r").value());
				std::cout << "distance: " << light->distance << std::endl;
			}
        }
        
        size_t dot = name.find(".");
        
        if (dot != std::string::npos) {
            group_name = name.substr(0,dot);
            lamp_name = name.substr(dot+1);
        }
        else {
            group_name = name;
            lamp_name = name;
        }
        
        light->name = lamp_name;

		std::cout << "group name: " << group_name << ", light name: " << light->name << std::endl;
		
		if (light->name == "anticollision_U") {
			light->f1 = 2.0f;
			light->f2 = 0.3f;
			light->emit = MDLLightEmit::EMITNO;
			light->halo = MDLLightHalo::HALOYES;
			//light->f3 = 1.5f;
		}
		else if (light->name == "anticollision_B") {
			light->f1 = 2.0f;
			light->f2 = 0.3f;
			light->emit = MDLLightEmit::EMITNO;
			light->halo = MDLLightHalo::HALOYES;
			//light->f3 = 1.5f;
		}
		else if (light->name == "strobe_1" || light->name == "strobe_2" || light->name == "strobe_3") {
			light->f1 = 3.0f;
			light->f2 = 0.3f;
			light->emit = MDLLightEmit::EMITNO;
			light->halo = MDLLightHalo::HALOYES;
			//light->f3 = 2.0f;
		}
		else if (group_name.compare("taxi") == 0) {
			light->f1 = -1;
			light->f2 = -1;
			light->f3 = -1;

			light->direction.x = 0;
			light->direction.y = 0;
			light->direction.z = 1;

			light->emit = MDLLightEmit::EMITYES;
			light->halo = MDLLightHalo::HALOYES;
		}
		else if (group_name.compare("landing") == 0) {
			light->f1 = -1;
			light->f2 = -1;
			light->f3 = -1;

			light->direction.x = 0;
			light->direction.y = 0;
			light->direction.z = 1;
			light->emit = MDLLightEmit::EMITYES;
			light->halo = MDLLightHalo::HALOYES;
		}
		else if (light->name == "wing_L" || light->name == "wing_R") {
			light->f1 = -1;
			light->f2 = -1;
			light->f3 = -1;
			light->emit = MDLLightEmit::EMITNO;
			light->halo = MDLLightHalo::HALOYES;
		}
		else if (group_name.find("rgl") != std::string::npos) {
			if (light->name.compare("0") == 0) {
				light->f1 = 0;
				light->f2 = 2;
				light->f3 = 2;
			}
			else {
				light->f1 = 2;
				light->f2 = 2;
				light->f3 = 0;
			}
		}
		else {
			light->f1 = -1;
			light->f2 = -1;
			light->f3 = -1;
			//light->halo = MDLLightHalo::HALOYES;
			//light->emit = MDLLightEmit::EMITNO;
		}

        
        MDLLightGroup* lg = lightfile->get_lightgroup(group_name);
        
        if (lg == NULL) {
            lg = new MDLLightGroup;
            lg->name = group_name;
            lightfile->groups.push_back(lg);
        }
        
        lg->lights.push_back(light);
    }
    
    return true;
}


bool MDLPKG::parse_textures(const std::string& folder_path, const pugi::xml_document& doc, bool make_callsigns, const std::string& package_name, bool b_url) {
    pugi::xml_node root = doc.child("nut");
    pugi::xml_node images = root.child("images");
    pugi::xml_node textures_xml = root.child("textures");
    
    for (auto t : textures_xml.children()) {
        if (strcmp(t.attribute("type").value(), "none") == 0)
            continue;
        
        std::string name = t.attribute("name").value();
        std::string filename = t.child("image").text().as_string();
        
        for (auto i : images.children()) {
            std::string img_name = i.attribute("name").value();
            
            if (img_name.compare(filename) == 0) {
				if (get_texture(name) != NULL) {
                    continue;
                }
                
                std::string url = i.attribute("url").value();
                
				std::string p = url.substr(0, url.find_last_of("\\/"));

                MDLTexture* tex = new MDLTexture;
				tex->path		= folder_path + (b_url ? p : "");
                tex->filename   = filename;
                tex->name       = name;
				tex->skip		= true;
                
#ifdef _WIN32
                tex->fullpath   = tex->path + "\\" + tex->filename;
				std::replace(tex->fullpath.begin(), tex->fullpath.end(), '/', '\\');
#else
                tex->fullpath   = tex->path + "/" + tex->filename;
#endif
				printf("tex dir: %s, file: %s...", tex->fullpath.c_str(), tex->filename.c_str());
				// check if file existstex->fullpath = "C:\\vmshare\\si_vis\\models\\aircraft\\nut\\//../textures/default\\default_d.dds"
				std::ifstream infile(tex->fullpath);
				if (!infile.good()) {

					printf("WARNING not found\n");
					delete tex;

					continue;
				}

                tex->filesize   = OSFuncs::get_filesize(tex->fullpath.c_str());
                
				MurmurHash3_x86_32(tex->filename.c_str(), (int)tex->filename.length(), 0, &tex->hash);

                textures.push_back(tex);
            }
        }
    }

	if (!make_callsigns)
		return true;
    


    return false;
}

void MDLPKG::set_node_positions (const pugi::xml_document& doc) {
    pugi::xml_node root = doc.child("nut");
    pugi::xml_node scene = root.child("scenes").child("scene");
    
    // loop through objects
    for (auto s : scene.children()) {
        
		std::string obj_name = s.attribute("name").as_string();

        if (strcmp(s.attribute("type").value(), "light") == 0) {
			MDLLight* light = lightfile->get_light(obj_name);
            
			if (light == NULL) {
				std::cout << "ERROR: light " << obj_name << " not found " << std::endl;
				App::quit();
			}

            for (auto rt : s.children()) {
                if (strcmp(rt.name(), "position") == 0) {
                    light->tx = atof(rt.attribute("x").as_string());
                    light->ty = atof(rt.attribute("y").as_string());
                    light->tz = atof(rt.attribute("z").as_string());
                }
				if (strcmp(rt.name(), "rotation") == 0) {
					light->direction.x = atof(rt.attribute("x").as_string());
					light->direction.y = atof(rt.attribute("y").as_string());
					light->direction.z = atof(rt.attribute("z").as_string());

					
					glm::vec3 d(light->direction.x, light->direction.y, light->direction.z);

					d = -d;

					light->direction.x = d.x;
					light->direction.y = d.y;
					light->direction.z = d.z;

				}
            }
        }
        if (strcmp(s.attribute("type").value(), "mesh") == 0) {
			MDLMesh* mesh = get_mesh_raw_name(obj_name);
            
			if (mesh == NULL)  {
				std::cout << "ERROR: mesh " << obj_name << " not found" << std::endl;
				App::quit();
			}

            for (auto rt : s.children()) {
                if (strcmp(rt.name(), "position") == 0) {
                    mesh->tx = atof(rt.attribute("x").as_string());
                    mesh->ty = atof(rt.attribute("y").as_string());
                    mesh->tz = atof(rt.attribute("z").as_string());
                }
				if (strcmp(rt.name(), "rotation_quat") == 0) {
					
					if (mesh->name.find("striped") != std::string::npos) {
						printf("stuff");
					}

					glm::quat q = glm::quat(atof(rt.attribute("w").as_string()), atof(rt.attribute("x").as_string()), atof(rt.attribute("y").as_string()), atof(rt.attribute("z").as_string()));

					mesh->rx = q.x;
					mesh->ry = q.y;
					mesh->rz = q.z;
					mesh->rw = q.w;
				}
                if (strcmp(rt.name(), "material") == 0) {
                    MDLMaterial* mat = get_material(rt.attribute("name").as_string());
                    
                    if (mat == NULL) {
                        std::cout << "ERROR: material " << rt.attribute("name").as_string() << " not found " << std::endl;
						App::quit();
                    }
                    
                    mesh->materials.push_back(mat);

					if (mat->name.compare("fans") == 0) {
						mat->properties |= MDLMatFlag::ALPHA_TEST;
						mat->properties |= MDLMatFlag::NO_CULLING;
					}

					if (mat->is_billboard) {
						if (mat->type == MDLMaterialType::MDL_MTL_TREES) {
							// lets randomize tree sizes

							// tree texture is divided in 4 squares
							float prop[4] = { 61.0f / 126.0f, 50.0f / 126.0f, 99.0f / 128.0f, 54.0f / 62.0f };
							int heights[4][2] = { { 8, 15 }, { 8, 15 }, { 8, 15 }, { 8, 15 } };


							for (int vi = 0; vi < mesh->vertices.size(); ++vi) {

								int index = rand() % 8;
								if (index <= 6) {
									index = 0;
								}
								else {
									index = 1;
								}
								
								float height = (float)(rand() % heights[index][1] + heights[index][0]);
								float width = height * prop[index];

								mesh->vertices[vi].billboard_data.push_back(width);
								mesh->vertices[vi].billboard_data.push_back(height);
								mesh->vertices[vi].billboard_data.push_back((float)index);
							}

							mesh->vbo_size = sizeof(float) * 6 * mesh->vertices.size();
						}
					}
                }
            }
        }
        if (strcmp(s.attribute("type").value(), "skeleton") == 0) {
            for (auto rt : s.children()) {
                if (strcmp(rt.name(), "position") == 0) {
                    skeleton->tx = atof(rt.attribute("x").as_string());
                    skeleton->ty = atof(rt.attribute("y").as_string());
                    skeleton->tz = atof(rt.attribute("z").as_string());
                }
				if (strcmp(rt.name(), "rotation") == 0) {
					skeleton->rx = atof(rt.attribute("x").as_string());
					skeleton->ry = atof(rt.attribute("y").as_string());
					skeleton->rz = atof(rt.attribute("z").as_string());
				}
            }
        }
    }
    
}

glm::mat4x4 MDLPKG::swap_mat4_zy (glm::mat4x4& in) {
    //glm::mat4x4 out = glm::transpose(in);
    glm::mat4x4 out = in;
    
    std::swap(out[0].y, out[0].z);
    std::swap(out[1].y, out[1].z);
    std::swap(out[2].y, out[2].z);
    std::swap(out[3].y, out[3].z);
    
    std::swap(out[1].x, out[2].x);
    std::swap(out[1].y, out[2].y);
    std::swap(out[1].z, out[2].z);
    std::swap(out[1].w, out[2].w);
    
    /*out[0].z *= -1;
    out[1].z *= -1;
    out[3].z *= -1;
    out[2].x *= -1;
    out[2].y *= -1;
    out[2].w *= -1;*/
    
    return out;
}

glm::mat4 MDLPKG::get_matrix (float tx, float ty, float tz, float rx, float ry, float rz, float rw) {
    glm::mat4x4 mat(1.0f);
    
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz));
    glm::quat quat(rw, rx, ry, rz);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1,1,1));
    
    //mat = scale * glm::mat4(quat) * trans;
	mat = trans * glm::mat4(quat) * scale;
    //mat = swap_mat4_zy(mat);
    
    return mat;
}

void MDLPKG::make_bone_abs_matrices (MDLBone* parent, MDLBone* bone) {
    if (bone == NULL) {
        std::cout << "ERROR: make_bone_abs_matrices NULL bone " << std::endl;
		App::quit();
    }
    
    glm::mat4x4 parent_mat(1.0f);
    
    if (parent != NULL) {
        parent_mat = parent->abs_matrix;
    }
    else {
        parent_mat = skeleton->matrix;
    }
    
    bone->abs_matrix = parent_mat * bone->matrix;
    
    for (auto b : skeleton->bones) {
        if (b->parent_index == bone->index) {
            make_bone_abs_matrices(bone, b);
        }
    }
}

void MDLPKG::post_parse() {
    
    /*std::vector<MDLTexture*> newtextures;
    
    for (auto m : materials) {
        MDLTexture* tex = NULL;
        
        for (auto t : textures) {
            if (m->texture == t) {
                tex = t;
            }
        }
        
        if (tex != NULL) {
            newtextures.push_back(tex);
        }
    }
    
    textures = newtextures;*/
}

void MDLPKG::make_matrices () {
	for (auto&& m : models) {
		glm::mat4x4 mat(1.0f);

		MDLMesh* submesh = m->submeshes[0];

		/*if (submesh->name.find("striped") != std::string::npos) {
			printf("stuff");
		}*/

		m->matrix = get_matrix(submesh->tx, submesh->ty, submesh->tz, submesh->rx, submesh->ry, submesh->rz, submesh->rw);

		//printf("stuff");
	}
	
	if (skeleton == NULL)
        return;
    
	glm::mat4x4 mat(1.0f);

	/*glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(skeleton->tx, skeleton->ty, skeleton->tz));
	glm::mat4 rot = glm::orientate4(glm::vec3(skeleton->rx, skeleton->ry, skeleton->rz));
	
	//glm::quat quat(rw, rx, ry, rz);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));

	mat = scale * rot * trans;
	//mat = swap_mat4_zy(mat);
	*/
	skeleton->matrix = mat;

    //skeleton->matrix = get_matrix(skeleton->tx, skeleton->ty, skeleton->tz, skeleton->rx, skeleton->ry, skeleton->rz, skeleton->rw);
    
    MDLBone* root = NULL;
    
    for (auto b : skeleton->bones) {
        if (b->parent_index == -1)
            root = b;
        
        // calculate local matrix
        b->matrix = get_matrix(b->tx, b->ty, b->tz, b->rx, b->ry, b->rz, b->rw);
    }
    
    make_bone_abs_matrices(NULL, root);
    
    for (auto b : skeleton->bones) {
        b->inv_bind_pose_matrix = glm::inverse(b->abs_matrix);

		std::cout << "inv" << std::endl;
    }
}

void MDLPKG::swap_vertices_zy () {
    for (auto mod : models) {
        for (auto sm : mod->submeshes) {
            for (int i = 0; i < sm->vertices.size(); ++i) {
                std::swap(sm->vertices[i].y, sm->vertices[i].z);
                sm->vertices[i].x = sm->vertices[i].x * -1;
                
                std::swap(sm->vertices[i].ny, sm->vertices[i].nz);
                sm->vertices[i].nx = sm->vertices[i].nx * -1;
            }
        }
    }

	if (skeleton == NULL)
		return;

	for (auto anim : skeleton->animations) {
		for (auto frame : anim->frames) {
			std::swap(frame->ty, frame->tz);
			std::swap(frame->ry, frame->rz);

			frame->tx *= -1;
			frame->rx *= -1;
		}
	}
}

void MDLPKG::print_textures () {
    std::cout << "textures: " << std::endl;
    
    for (auto t : textures) {
        std::cout << "texture: " << t->path << ", filename: " << t->filename << ", filesize: " << t->filesize << std::endl;
    }
}

void MDLPKG::print_models () {
    std::cout << "models: " << std::endl;
    
    for (auto m : models) {
        m->print();
    }
}

//
//  mdl.hpp
//  nut_to_mdl
//
//  Created by macbookpro on 2016-03-21.
//  Copyright © 2016 macbookpro. All rights reserved.
//

#ifndef mdl_hpp
#define mdl_hpp

#include "pugixml.hpp"


#include <string>
#include <vector>
#include <iostream>

#include <sstream>
#include <algorithm>
#include <iterator>
#include <utility>


#include "glm/glm.hpp"
#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

const static uint32_t MDL_VERSION = 2;

const static char * MDLTYPE_NAMES[] = { "SIMPLE", "SKIN", "AV_MODEL", "AV_CS", "FLARE_FX", "SKY", "SKY_OBJ", "BG_FOG", "STARS", "CIRRUS", "STATIC_DECAL", "BUILDING", "GENERAL", "TREES" };

class App {
public:
	
	static void quit();
};

enum MDLLightHalo {
	HALONO = 0,
	HALOYES = 1
};

enum MDLLightEmit {
	EMITNO = 0,
	EMITYES = 1
};

enum MDLFlag {
	SHADOW = 0x01,
	ENVIRONMENT = 0x02,
	REFLECTION = 0x04
	//0x08
};

enum MDLMatFlag {
	NO_CULLING = 0x01, // binary 0001
	ALPHA_TEST = 0x02, // binary 0010
	ALPHA_BLEND = 0x04, // binary 0100
	D = 0x08, // binary 1000
};

enum MDLAircraftLightGroup {
    LIGHT_NAVIGATION = 0,
    LIGHT_TAXI,
    LIGHT_LANDING
};

enum MDLLightType {
    MDL_LIGHT_SPOT = 0,
    MDL_LIGHT_LAMP,
    MDL_LIGHT_POINTSPRITE
};

enum MDLMaterialType {
    MDL_MTL_NONE = -1,
    MDL_MTL_SIMPLE = 0,
    MDL_MTL_SKIN,
    MDL_MTL_AV_MODEL,
    MDL_MTL_AV_CS,
    MDL_MTL_FLARE_FX,
    MDL_MTL_SKY,
    MDL_MTL_SKY_OBJ,
    MDL_MTL_BG_FOG,
    MDL_MTL_STARS,
    MDL_MTL_CIRRUS,
    MDL_MTL_STATIC_DECAL,
    MDL_MTL_BUILDING,
	MDL_MTL_GENERAL,
	MDL_MTL_TREES
};

struct MDLBone {
	std::string name;
	int index;
	int parent_index;
	int file_index;

	bool parsed;

	MDLBone();

	float tx, ty, tz;
	float rx, ry, rz, rw;

	glm::vec3 translation;
	glm::quat rotation;

	glm::mat4x4 matrix;
	glm::mat4x4 abs_matrix;
	glm::mat4x4 inv_bind_pose_matrix;
};

bool bonesort(const MDLBone* key1, const MDLBone* key2);

struct MDLVertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
    uint32_t b;
    float w;

	std::vector<float> billboard_data;

	MDLBone* bone;

	std::string bone_name;
    
    bool bones;
};

struct MDLTexture {
	MDLTexture();

    std::string name;
    std::string extension;
    std::string filename;
    std::string path;
    std::string fullpath;
    
    unsigned int filesize;
    unsigned int hash;

	bool skip;
};

struct MDLMaterial {
    std::string         name;
    float               shininess;
	float				alpha;
	unsigned int		properties;
	bool				is_billboard;
	float				zbias;

	std::vector<MDLTexture*> textures;
    MDLMaterialType     type;
    unsigned int        hash;

	bool				zero_hash;

	// legacy
	MDLTexture* get_material_propery_texture();
	MDLTexture* get_surface_texture();

	// actual
	MDLTexture* get_diffuse_texture();
	MDLTexture* get_lights_texture();
	MDLTexture* get_property_texture();

};

struct MDLLight {
    float r, g, b;
    float att_l, att_q;
    
    MDLLightType type;
    
	MDLLightHalo halo;
	MDLLightEmit emit;

	float distance;
	float lamp_size;
	float power;
	//float shape_degrees;
	//float shape_blend;

	float spot_inner_angle;
	float spot_outer_angle;

	float spot_offset;

    float tx, ty, tz;
    float rx, ry, rz, rw;
    
    float f1, f2, f3;
    
    glm::mat4x4 matrix;
    
    std::string name;
    std::string raw_name;

	glm::vec3 direction;
};

struct MDLLightGroup {
    std::vector<MDLLight*> lights;
    
    std::string name;
};

struct MDLLightFile {
    std::vector<MDLLightGroup*> groups;
    std::string filename;
    
    MDLLightGroup* get_lightgroup (const std::string& name);
    
    MDLLight* get_light (const std::string& name);
    
};

struct MDLMesh {

    std::string name;
    std::string raw_name;
    
    std::vector<MDLMaterial*> materials;
    
    float tx, ty, tz;
    float rx, ry, rz, rw;
    
	bool is_billboard;

    glm::mat4x4 matrix;

	unsigned int material_flags;
    
    unsigned int vbo_size;
    unsigned int indice_size;
    
    std::vector<MDLVertex> vertices;
    std::vector<unsigned int> indices;
    
    MDLMesh ();
    
    bool write_tex_hash ();
    
    std::vector<float> get_bounding_box ();
    
    
    void print ();
};

struct MDLModel {
    std::vector<MDLMesh*> submeshes;
    
    std::string name;
    std::string filename;
    
    glm::mat4x4 matrix;

	unsigned int properties;
    
    std::vector<float> get_bounding_box ();
    void print ();
};

struct MDLAnimationFrame {
    int time;
    int bone_index;
    float tx, ty, tz;
    float rx, ry, rz, rw;
    
	MDLBone* bone;

	glm::quat rotation;
	glm::vec3 translation;

    glm::mat4x4 matrix;
};

struct MDLAnimation {
    std::string filename;
    std::string name;
    
	std::string source;

    std::vector<MDLAnimationFrame*> frames;
    
    std::vector<MDLAnimationFrame*> get_frames_for_bone (int bone_index);
	void erase_frames_for_bone(int i);

    std::vector<int> get_bones ();
    
    MDLAnimationFrame* get_frame_for_time_bone (int time, int bone_index);
    void print ();
};

struct MDLSkeleton {
    std::string filename;
    
    std::vector<MDLBone*> bones;
    std::vector<MDLAnimation*> animations;
    
    std::string name;
    std::string raw_name;
    
	int parse_count;

    unsigned int num_bones;
    
    float tx, ty, tz;
    float rx, ry, rz, rw;
    
    glm::mat4x4 matrix;
    
    MDLSkeleton ();
    
	bool cleanup();

    bool add_bone (MDLBone* bone);
    
    glm::mat4x4 get_parent_abs_mat (const std::string& bone_name);
    
    MDLBone* get_bone (const std::string& name);
    
    MDLBone* get_bone_by_index (int index);
	MDLBone* get_bone_by_file_index(int index);
    
    void print_animations ();
    
    void print_skeleton ();
};

struct MDLPKG {
    MDLSkeleton*                skeleton;
    std::vector<MDLModel*>      models;
    std::vector<MDLTexture*>    textures;
    std::vector<MDLMaterial*>   materials;
    
    MDLLightFile*               lightfile;
    std::string filename;
    std::string area_filename;
    
    MDLPKG ();
    
    void post_parse ();
    
	void cleanup_animation(MDLAnimation* animation);

    MDLMaterial* get_material (const std::string& name);
    
    MDLTexture* get_texture (const std::string& name);
    
    MDLMesh* get_mesh_raw_name (const std::string& raw_name);
    
    MDLModel* get_model (std::string &prefix);
    
    std::vector<std::string> get_values (std::string& str);
    
	bool parse_geometry(const pugi::xml_document& doc, const std::string& mdl_prefix, bool cast_shadow);
    
	bool parse_skeleton(const std::string& prefix, const pugi::xml_document& doc);
    
    bool parse_bones (const pugi::xml_node& bone, MDLBone* parent);
    
	bool parse_animations(const std::string& prefix, const pugi::xml_document& doc);
    
    bool parse_materials (const pugi::xml_document& doc);
    
	bool parse_lights(const std::string& prefix, const pugi::xml_document& doc, float default_lightsize);
    
	bool parse_textures(const std::string& folder_path, const pugi::xml_document& doc, bool make_callsigns, const std::string& package_name, bool b_url);
    
    void set_node_positions (const pugi::xml_document& doc);

	
    glm::mat4x4 swap_mat4_zy (glm::mat4x4& in);
    
    glm::mat4 get_matrix (float tx, float ty, float tz, float rx, float ry, float rz, float rw);
    
    void make_bone_abs_matrices (MDLBone* parent, MDLBone* bone);
    
    void make_matrices ();
    
    void swap_vertices_zy ();
    
    void print_textures ();
    
    void print_models ();

	std::vector<std::string> files_in_folder(const std::string& path);
};


#endif /* mdl_hpp */

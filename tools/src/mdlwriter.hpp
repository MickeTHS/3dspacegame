#pragma once

#include <iostream>
#include <vector>
#include <string>

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


enum MDLFileType {
    MDL_NONE = 0,
    MDL_TEXTURE,
    MDL_SKELETON,
    MDL_ANIMATION,
    MDL_LIGHTS,
    MDL_MDL,
    MDL_AREA
};

struct MDLFile {
	MDLFile();

    std::string fullpath;
    std::string filename;
    unsigned int filesize;
    MDLFileType type;

	bool skip;
};

bool filesort(const MDLFile* key1, const MDLFile* key2);

struct MDLAnimation;
struct MDLTexture;
struct MDLSkeleton;
struct MDLModel;
struct MDLLightFile;
struct MDLPKG;

struct MDLWriter {
    
	std::vector<MDLFile*> get_files_to_write();

    void sort_files ();
    
    bool write_pkg (const std::string& path, const std::string& filename);
    
    MDLFile* write_area (const std::string& path, MDLPKG* pkg);
    
    MDLFile* write_lights (const std::string& path, MDLLightFile* lightfile);
    
    void write_texture (const std::string& path, MDLTexture* texture);
    
    void write_mdl (const std::string& path, MDLModel* model/*, const std::string& mdl_prefix*/);
    
    void write_skeleton (const std::string& path, MDLSkeleton* skeleton);
    
    void write_animation (const std::string& path, MDLAnimation* animation);

	void print_matrix(glm::mat4x4& mat);
	
	void quit();

private:
	std::vector<MDLFile*> files;
    std::vector<std::string> _filenames;
};

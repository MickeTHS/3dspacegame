//
//  main.cpp
//  nut_to_mdl
//
//  Created by macbookpro on 2016-03-18.
//  Copyright © 2016 macbookpro. All rights reserved.
//
#include "pugixml.hpp"
#include <iostream>
#include <vector>
#include <string>

#include "mdl.hpp"
#include "mdlwriter.hpp"
#include "osfuncs.h"
#include "log.h"
#include "animreader.h"

FILE *Log::log_file;

void print_usage () {
    std::cout << "nut_to_mdl converter" << std::endl << "Author: Michael Nilsson" << std::endl;
    std::cout << std::endl << "arguments: <folder path or .nut file> <out path>" << std::endl;
    std::cout << " optional: " << std::endl << "  -area <areaname>: write area with given name " << std::endl;
	std::cout << "  -shadow: object will cast a shadow" << std::endl;
	std::cout << "  -mdlprefix <string prefix>: unique prefix to put for each model" << std::endl;
	std::cout << "  -lightsize <float light size radius>: set all lights without <size> tag to given size" << std::endl;
	std::cout << "  -keypress: force keypress after successful conversion" << std::endl;
	std::cout << "  -lightsname <lightfile prefix>: write lights file with given prefix" << std::endl;
	std::cout << "  -nourl: strip the image URL tag to find images, only look in local or -t texture folder" << std::endl;
    std::cout << "  -pkg <filename>: write pkg file with given name" << std::endl;
	std::cout << "  -callsign: create individual callsigns from textures located in source folder " << std::endl;
	std::cout << "  -t <folder>: texture folder to look for textures " << std::endl;
    std::cout << " example: " << std::endl << "  nut_to_mdl /Users/folder/ . " << std::endl;
    std::cout << "  nut_to_mdl /Users/folder/file.nut models/ -area riga" << std::endl;
    std::cout << "  nut_to_mdl /Users/folder / -pkg lotsofairplanes" << std::endl;
    std::cout << "  nut_to_mdl /Users/folder/buildings.nut /absolute/path/models/ -area riga -pkg buildings" << std::endl;
}

int main (int argc, const char * argv[]) {
	loginit();
	
	MDLPKG pkg;
    
    if (argc <= 3) {
        print_usage();
		
        exit(0);
    }

	bool force_keypress = false;
	bool url = true;
	bool use_lights_prefix = false;
	float default_lightsize = 0.1f;
    
    //std::string outpath = "/Users/macbookpro/modelling/mdl/";
    std::string inpath = std::string(argv[1]);
    
    std::string path;
    std::string infile = "";
    std::string outpath = std::string(argv[2]);
    
	std::string mdl_prefix = "";
    std::string area_name = "";
    std::string package_name = "";
	std::string arg_folder = "";
	std::string texture_path = "";
	std::string lights_prefix = "";

	bool make_callsigns = false;
	bool cast_shadow = false;
    
    
    if (inpath.find(".nut") != std::string::npos) {
        path = inpath.substr(0, inpath.find_last_of("\\/")) + OSFuncs::get_folder_char();;
        infile = inpath.substr(inpath.find_last_of("\\/")+1);
    }
    else {
        path = inpath + OSFuncs::get_folder_char();
    }
    
	texture_path = path;
    
    for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "-lightsize") == 0) {
			default_lightsize = atof(argv[i + 1]);
		}
		if (strcmp(argv[i], "-shadow") == 0) {
			cast_shadow = true;
		}
		if (strcmp(argv[i], "-keypress") == 0) {
			force_keypress = true;
		}
		if (strcmp(argv[i], "-lightsname") == 0) {
			use_lights_prefix = true;
			lights_prefix = std::string(argv[i + 1]);
		}
		if (strcmp(argv[i], "-mdlprefix") == 0) {
			mdl_prefix = std::string(argv[i + 1]);
		}
		if (strcmp(argv[i], "-nourl") == 0) {
			url = false;
		}
		if (strcmp(argv[i], "-t") == 0) {
			if (argc == i + 1) {
				print_usage();
				exit(0);
			}

			texture_path = std::string(argv[i + 1]);
		}
        if (strcmp(argv[i], "-area") == 0) {
            if (argc == i+1) {
                print_usage();
                exit(0);
            }
            
            area_name = std::string(argv[i+1]);
        }
        if (strcmp(argv[i], "-pkg") == 0) {
            if (argc == i+1) {
                print_usage();
                exit(0);
            }
            
            package_name = std::string(argv[i+1]);
        }
		if (strcmp(argv[i], "-callsigns") == 0) {
			make_callsigns = true;
		}
    }
    
    std::string current_file = std::string(path + infile);
    
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(current_file.c_str());
    
	std::cout << "Reading " << current_file << std::endl;
    
    if (result.status != pugi::xml_parse_status::status_ok) {
        std::cout << "ERROR! " << result.description() << std::endl;
		getchar();
        return 1;
    }
    
    pkg.filename = package_name;

	std::string prefix = package_name.substr(0, package_name.find("_"));
    
	printf("parsing lights ...\n");
	pkg.parse_lights(use_lights_prefix ? lights_prefix : prefix, doc, default_lightsize);
	printf("parsing textures in folder %s...\n", texture_path.c_str());
	pkg.parse_textures(texture_path, doc, make_callsigns, prefix, url);
	printf("parsing materials ...\n");
    pkg.parse_materials(doc);

	printf("parsing skeleton ...\n");
	pkg.parse_skeleton(prefix + "_", doc);
	printf("parsing geometries ...\n");
	pkg.parse_geometry(doc, mdl_prefix, cast_shadow);
	printf("parsing animations ...\n");
	pkg.parse_animations(prefix + "_", doc);
	//AnimReader read;

	//read.read_animations("C:\\vmshare\\si_vis\\models\\aircraft\\nut\\dh8d.dae.anim", &pkg);
	printf("setting node positions ...\n");
    pkg.set_node_positions(doc);
	printf("making matrices ...\n");
    pkg.make_matrices();
    //pkg.swap_vertices_zy();
    pkg.post_parse();
    
    if (pkg.skeleton != NULL) {
        pkg.skeleton->print_animations();
        pkg.skeleton->print_skeleton();
    }
    pkg.print_textures();
    pkg.print_models();
    
    MDLWriter writer;
    
    for (auto t : pkg.textures) {
        writer.write_texture(outpath, t);
    }
    
    for (auto m : pkg.models) {
		writer.write_mdl(outpath, m/*, mdl_prefix*/);
    }
    
    if (pkg.skeleton != NULL) {
        writer.write_skeleton(outpath, pkg.skeleton);

        for (auto anim : pkg.skeleton->animations) {
            writer.write_animation(outpath, anim);
        }
    }
    
    if (pkg.lightfile != NULL) {
        MDLFile* file = writer.write_lights(outpath, pkg.lightfile);
        
        std::cout << "Wrote lights " << file->fullpath << ", filesize: " << file->filesize << std::endl;
    }
    
    if (area_name.compare("") != 0) {
        pkg.area_filename = area_name + ".area";
        
        MDLFile* file = writer.write_area(outpath, &pkg);
        
        std::cout << "Wrote area " << file->fullpath << ", filesize: " << file->filesize << std::endl;
    }
    
    std::cout << "Making PKG ..." << std::endl;
    
    writer.write_pkg(outpath, package_name + ".arc");
	
    std::cout << "Done: " << package_name << ".arc" << std::endl;

	if (force_keypress) {
		getchar();
	}
	
	logclose();

    return 0;
}

//C:\vmshare\si_vis\models\aircraft\nut\dh8d.nut C:\vmshare\si_vis\models\aircraft\pkg\ -pkg dh8d
//C:\vmshare\si_vis\models\aircraft\release1\a321_aus.nut C:\vmshare\si_vis\models\aircraft\pkg\ - pkg a321_aus
//C:\vmshare\si_vis\models\buildings\nut\terrain.nut C:\vmshare\si_vis\models\buildings\pkg\ -pkg riga -t C:\vmshare\si_vis\models\buildings\textures\terrain\


//terrain
//C:\vmshare\si_vis\models\buildings\nut\terrain.nut C:\vmshare\si_vis\models\buildings\pkg\ -pkg terrain -t C:\vmshare\si_vis\models\buildings\textures\terrain\ -area riga_terrain


//city
//C:\vmshare\si_vis\models\buildings\nut\city.nut C:\vmshare\si_vis\models\buildings\pkg\ -pkg riga_city -t C:\vmshare\si_vis\models\buildings\textures\city\ -area riga_city
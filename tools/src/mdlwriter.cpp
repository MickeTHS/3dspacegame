//
//  mdlwriter.cpp
//  nut_to_mdl
//
//  Created by macbookpro on 2016-03-21.
//  Copyright © 2016 macbookpro. All rights reserved.
//

#include "mdlwriter.hpp"
#include "osfuncs.h"
#include "mdl.hpp"

#include "log.h"

#include <engine/data_file.h>
#include <engine/data_archive.h>

bool filesort(const MDLFile *key1, const MDLFile *key2)
{
    return (int)key1->type < (int)key2->type;
}

MDLFile::MDLFile()
{
    skip = false;
}

void MDLWriter::sort_files()
{
    std::sort(files.begin(), files.end(), filesort);
}

bool MDLWriter::write_pkg(const std::string &path, const std::string &filename)
{
    Data_archive arc;

    for (auto s : _filenames) {
        arc.read_file_and_add(s.c_str());
    }

    std::string fullpath = "";

#ifdef _WIN32
    fullpath = path + "\\" + filename;
#else
    fullpath = path + "/" + filename;
#endif

    arc.write(fullpath.c_str());

    return true;
}

MDLFile *MDLWriter::write_lights(const std::string &path, MDLLightFile *lightfile)
{
    MDLFile *file = new MDLFile;
    std::string destination;

#ifdef _WIN32
    destination = path + "\\" + lightfile->filename;
#else
    destination = path + "/" + lightfile->filename;
#endif

    FILE *f = fopen(destination.c_str(), "wb");

    size_t written = 0;

    std::string file_str = "";

    for (auto group : lightfile->groups)
    {
        file_str += " " + group->name + " = {\n";
        file_str += " name = '" + group->name + "'\n";
        //written += fwrite(line.c_str(), 1, line.length() * sizeof(char), f);

        for (auto light : group->lights)
        {
            file_str += "    " + light->name + " = {\n";
            //written += fwrite(line.c_str(), 1, line.length() * sizeof(char), f);

            file_str += "       halo = i" + std::to_string(light->halo) + "\n";
            file_str += "       emit = i" + std::to_string(light->emit) + "\n";

            if (light->type == MDLLightType::MDL_LIGHT_LAMP)
            {
                file_str += "       type = i1\n";
            }
            else if (light->type == MDLLightType::MDL_LIGHT_SPOT)
            {
                file_str += "       type = i2\n";
                //file_str += "       linear_attenuation = f" + std::to_string(light->att_l) + "\n";
                file_str += "       direction = f[" + std::to_string(light->direction.x) + "," + std::to_string(light->direction.y) + "," + std::to_string(light->direction.z) + "]\n";
                file_str += "       inner_angle = f" + std::to_string(light->spot_inner_angle) + "\n";
                file_str += "       outer_angle = f" + std::to_string(light->spot_outer_angle) + "\n";
                file_str += "       offset = f" + std::to_string(light->spot_offset) + "\n";
            }
            else if (light->type == MDLLightType::MDL_LIGHT_POINTSPRITE)
            {
                file_str = "       type = i3\n";
            }

            //written += fwrite(line.c_str(), 1, line.length() * sizeof(char), f);
            file_str += "       power = f" + std::to_string(light->power) + "\n";
            file_str += "       distance = f" + std::to_string(light->distance) + "\n";
            file_str += "       size = f" + std::to_string(light->lamp_size) + "\n";
            file_str += "       position = f[" + std::to_string(light->tx) + "," + std::to_string(light->ty) + "," + std::to_string(light->tz) + "]\n";
            file_str += "       frequencies = f[" + std::to_string(light->f1) + "," + std::to_string(light->f2) /*+ "," + std::to_string(light->f3) */ + "]\n";
            file_str += "       color = f[" + std::to_string(light->r) + "," + std::to_string(light->g) + "," + std::to_string(light->b) + "]\n";
            file_str += "       name = '" + light->name + "'\n";
            file_str += "    }\n";
            //written += fwrite(line.c_str(), 1, line.length() * sizeof(char), f);
        }

        file_str += " }\n";
        //written += fwrite(line.c_str(), 1, line.length() * sizeof(char), f);
    }

    written = fwrite(file_str.c_str(), 1, file_str.length() * sizeof(char) + 1, f);

    fclose(f);

    file->filesize = (unsigned int)written;
    file->fullpath = destination;
    file->type = MDLFileType::MDL_LIGHTS;
    file->filename = lightfile->filename;

    files.push_back(file);

    return file;
}

MDLFile *MDLWriter::write_area(const std::string &path, MDLPKG *pkg)
{
    MDLFile *file = new MDLFile;
    std::string destination;

    std::string filename = pkg->area_filename;

#ifdef _WIN32
    destination = path + "\\" + filename;
#else
    destination = path + "/" + filename;
#endif

    FILE *f = fopen(destination.c_str(), "wb");

    size_t written = 0;

    std::string line = "";

    for (auto m : pkg->models)
    {
        line += m->filename + "_ = {\n";
        line += "   model = '" + m->filename + "'\n";
        line += "   transform = f[";

        for (int i = 0; i < 4; ++i)
        {
            line += std::to_string(m->matrix[i].x) + "," + std::to_string(m->matrix[i].y) + "," + std::to_string(m->matrix[i].z) + "," + std::to_string(m->matrix[i].w) + (i == 3 ? "" : ",");
        }

        line += "]\n";
        line += "}\n";
    }

    line += "\0";

    written += fwrite(line.c_str(), 1, sizeof(char) * line.length() + 1, f);

    file->fullpath = destination;
    file->type = MDLFileType::MDL_AREA;
    file->filename = filename;
    file->filesize = (unsigned int)written;

    fclose(f);

    files.push_back(file);

    return file;
}

void MDLWriter::write_texture(const std::string &path, MDLTexture *texture)
{
    Data_disk_file df;
    memset(&df.header, 0, sizeof(Data_disk_file_header));

    
    df.header.filetype = Data_filetype::DDS;

    Data_texture dm;
    memset(&dm.header, 0, sizeof(Data_texture_header));

    std::string destination;

#ifdef _WIN32
    destination = path + "\\" + texture->filename;
#else
    destination = path + "/" + texture->filename;
#endif

    OSFuncs::copy_file(texture->fullpath.c_str(), destination.c_str());
    dm.header.datasize = OSFuncs::get_filesize(destination.c_str());
    dm.data = (uint8_t*)malloc(dm.header.datasize);
    FILE *f = fopen(destination.c_str(), "rb");
    //fread(&dm.header, sizeof(Data_texture_header), 1, f);
    fread(dm.data, dm.header.datasize, 1, f);

    fclose(f);

    df.header.filesize = sizeof(Data_disk_file_header) + sizeof(Data_texture_header) + dm.header.datasize;

    std::string mdlfilename = destination.substr(destination.find_last_of("\\/") + 1);
    std::string prefix = mdlfilename.substr(0, mdlfilename.find_last_of("."));
    mdlfilename = prefix + ".ddx";
    strcpy(df.header.filename, mdlfilename.c_str());
    std::string mdlpath = destination.substr(0, destination.find_last_of("\\/"));
    std::string final_abs_path = mdlpath + mdlfilename;

    FILE *fp_mdl2 = fopen(final_abs_path.c_str(), "wb");
    fwrite(&df.header, sizeof(Data_disk_file_header), 1, fp_mdl2);
    fwrite(&dm.header, sizeof(Data_texture_header), 1, fp_mdl2);
    fwrite(dm.data, dm.header.datasize, 1, fp_mdl2);

    _filenames.push_back(final_abs_path);

    fclose(fp_mdl2);

}

std::vector<MDLFile *> MDLWriter::get_files_to_write()
{
    std::vector<MDLFile *> f;

    for (auto fi : files)
    {
        if (!fi->skip)
            f.push_back(fi);
    }

    return f;
}

void MDLWriter::quit()
{
    logmsg("error occured, quitting, see ee.log for details\n");

    logclose();

    exit(0);
}

void MDLWriter::write_mdl(const std::string &path, MDLModel *model /*, const std::string& mdl_prefix*/)
{
    Data_disk_file df;
    memset(&df.header, 0, sizeof(Data_disk_file_header));

    strcpy(df.header.filename, model->filename.c_str());
    df.header.filetype = Data_filetype::MODEL;

    Data_model dm;
    memset(&dm.header, 0, sizeof(Data_model_header));
    

    std::string filename = model->filename;
    std::string fullpath = "";
#ifdef _WIN32
    fullpath = path + "\\" + model->filename;
#else
    fullpath = path + "/" + model->filename;
#endif
        
    size_t written = 0;

    std::vector<float> bb = model->get_bounding_box();

    std::cout << "BoundingBox: [" << bb[0] << ", " << bb[1] << "," << bb[2] << "][" << bb[3] << "," << bb[4] << "," << bb[5] << "]" << std::endl;
    
    memcpy(&dm.header.bb[0], &bb[0], bb.size() * sizeof(float));
    dm.header.rendertype = Gfx_RT_type::RT_3D_POS_NORM_UV;
    
    for (auto s : model->submeshes)
    {
        MDLTexture *diffuse = s->materials[0]->get_diffuse_texture();
        dm.header.diffuse_id = diffuse->hash;
       
        dm.header.vertices_data_size = s->vbo_size;

        uint32_t ss = dm.header.vertices_data_size / (sizeof(float) * (3 + 3 + 2) + sizeof(uint32_t));

        dm.header.indices_data_size = s->indices.size() * sizeof(uint32_t);
        dm.verticesdata = malloc(dm.header.vertices_data_size);
        dm.indicedata = malloc(dm.header.indices_data_size);

        //written += fwrite(&s->materials[0]->type, sizeof(unsigned int), 1, f);
        uint32_t p = 0;

        for (auto v : s->vertices)
        {
            memcpy(&((uint8_t*)dm.verticesdata)[p], &v.x, sizeof(float)); p += sizeof(float);
            memcpy(&((uint8_t*)dm.verticesdata)[p], &v.y, sizeof(float)); p += sizeof(float);
            memcpy(&((uint8_t*)dm.verticesdata)[p], &v.z, sizeof(float)); p += sizeof(float);

            memcpy(&((uint8_t*)dm.verticesdata)[p], &v.nx, sizeof(float)); p += sizeof(float);
            memcpy(&((uint8_t*)dm.verticesdata)[p], &v.ny, sizeof(float)); p += sizeof(float);
            memcpy(&((uint8_t*)dm.verticesdata)[p], &v.nz, sizeof(float)); p += sizeof(float);

            memcpy(&((uint8_t*)dm.verticesdata)[p], &v.u, sizeof(float)); p += sizeof(float);
            memcpy(&((uint8_t*)dm.verticesdata)[p], &v.v, sizeof(float)); p += sizeof(float);

            if (v.bones) {
                dm.header.rendertype = Gfx_RT_type::RT_3D_POS_NORM_UV_BONE;
                memcpy(&((uint8_t*)dm.verticesdata)[p], &v.bone->index, sizeof(uint32_t)); p += sizeof(uint32_t);
            }

            if (s->is_billboard)
            {
                for (auto bbf : v.billboard_data)
                {
            //        written += fwrite(&bbf, 1, sizeof(float), f);
                }

                continue;
            }
        }

        assert(p == dm.header.vertices_data_size);

        
        uint32_t ip = 0;

        for (uint32_t i : s->indices)
        {
            memcpy(&((uint8_t*)dm.indicedata)[ip], &i, sizeof(uint32_t));
            ip += sizeof(uint32_t);
        }

        assert(ip == dm.header.indices_data_size);
    }

    df.header.filesize = sizeof(Data_model_header) + sizeof(Data_disk_file_header);
    df.header.filesize += dm.header.vertices_data_size + dm.header.indices_data_size;
    
    std::string mdlfilename = fullpath.substr(fullpath.find_last_of("\\/")+1);
    std::string mdlpath = fullpath.substr(0, fullpath.find_last_of("\\/"));
    std::string final_abs_path = mdlpath + mdlfilename;
    //std::string final_abs_path = "D:\\projects\\22_mdl.mdl";

    FILE *fp_mdl2 = fopen(final_abs_path.c_str(), "wb");
    fwrite(&df.header, sizeof(Data_disk_file_header), 1, fp_mdl2);
    fwrite(&dm.header, sizeof(Data_model_header), 1, fp_mdl2);
    fwrite(dm.verticesdata, dm.header.vertices_data_size, 1, fp_mdl2);
    fwrite(dm.indicedata, dm.header.indices_data_size, 1, fp_mdl2);

    float last_3[3];
    float first_3_floats[3];
    memcpy(first_3_floats, &((uint8_t*)dm.verticesdata)[0], sizeof(float) * 3);
    memcpy(last_3, &((uint8_t*)dm.verticesdata)[dm.header.vertices_data_size-sizeof(float)*3], sizeof(float)*3);

    uint32_t first_3[3];
    memcpy(first_3, &((uint8_t*)dm.indicedata)[0], sizeof(uint32_t) * 3);

    _filenames.push_back(final_abs_path);

    fclose(fp_mdl2);    
}

void MDLWriter::print_matrix(glm::mat4x4 &mat)
{
    size_t w = 0;

    for (int i = 0; i < 4; ++i)
    {
        std::cout << " " << mat[i].x << " " << mat[i].y << " " << mat[i].z << " " << mat[i].w << std::endl;
    }
}


void MDLWriter::write_skeleton(const std::string &path, MDLSkeleton *skeleton)
{

    Data_disk_file df;
    memset(&df.header, 0, sizeof(Data_disk_file_header));

    strcpy(df.header.filename, skeleton->filename.c_str());
    df.header.filetype = Data_filetype::SKELETON;

    Data_skeleton dm;
    memset(&dm.header, 0, sizeof(Data_skeleton_header));
    dm.header.num_bones = skeleton->num_bones;
       

    std::string fullpath = "";

    
#ifdef _WIN32
    fullpath = path + "\\" + skeleton->filename;
#else
    fullpath = path + "/" + skeleton->filename;
#endif
    
    int count = 0;

    for (auto b : skeleton->bones)
    {
        Data_bone mdlbone;
        memset(&mdlbone.name, 0, MDL_BONE_NAME_LENGTH);
        strncpy(&mdlbone.name[0], b->name.c_str(), 32);
        mdlbone.parent_index = b->parent_index;
        mdlbone.inv_bind_pose = b->inv_bind_pose_matrix;
        mdlbone.matrix = b->matrix;

        dm.bones.push_back(mdlbone);
    }

    
    df.header.filesize = sizeof(Data_skeleton_header) + sizeof(Data_disk_file_header);
    df.header.filesize += dm.bones.size() * sizeof(Data_bone);

    std::string mdlfilename = fullpath.substr(fullpath.find_last_of("\\/") + 1);
    std::string mdlpath = fullpath.substr(0, fullpath.find_last_of("\\/"));
    std::string abs_path = mdlpath + mdlfilename;

    FILE *fp_mdl2 = fopen(abs_path.c_str(), "wb");
    fwrite(&df.header, sizeof(Data_disk_file_header), 1, fp_mdl2);
    fwrite(&dm.header, sizeof(Data_skeleton_header), 1, fp_mdl2);
    fwrite(&dm.bones[0], dm.bones.size() * sizeof(Data_bone), 1, fp_mdl2);

    _filenames.push_back(abs_path);

    fclose(fp_mdl2);
}

void MDLWriter::write_animation(const std::string &path, MDLAnimation *animation)
{

    Data_disk_file df;
    memset(&df.header, 0, sizeof(Data_disk_file_header));

    strcpy(df.header.filename, animation->filename.c_str());
    df.header.filetype = Data_filetype::SKELETON;

    Data_animation dm;
    memset(&dm.header, 0, sizeof(Data_animation_header));
    

    std::string fullpath = "";
#ifdef _WIN32
    fullpath = path + "\\" + animation->filename;
#else
    fullpath = path + "/" + animation->filename;
#endif
    
    std::vector<int> bones = animation->get_bones();

    unsigned int num_bones = (unsigned int)bones.size();

    size_t written = 0;

    dm.header.num_bones = num_bones;
   
    unsigned int previous_frame_offset = 0;

    for (auto bi : bones)
    {
        unsigned int frame_count = (unsigned int)animation->get_frames_for_bone(bi).size();
        
        Data_animation_bone ab;
        ab.bone_index = bi;
        // frame count for this bone
        ab.frame_count = frame_count;
        ab.frame_offset = previous_frame_offset;

        dm.bones.push_back(ab);

        previous_frame_offset += frame_count;
    }

    unsigned int frame_count = (unsigned int)animation->frames.size();

    //std::cout << " total frame count: " << frame_count << std::endl;

    dm.header.num_frames = frame_count;

    for (auto fr : animation->frames)
    {
        Data_animation_frame daf;

        float tmp_time = (float)fr->time / 30.0f;

        daf.time = tmp_time;
        daf.translation = fr->translation;
        daf.rotation = fr->rotation;
        
        dm.frames.push_back(daf);
    }

    df.header.filesize = sizeof(Data_animation_header) + sizeof(Data_disk_file_header);
    df.header.filesize += dm.bones.size() * sizeof(Data_animation_bone);
    df.header.filesize += dm.frames.size() * sizeof(Data_animation_frame);

    std::string mdlfilename = fullpath.substr(fullpath.find_last_of("\\/") + 1);
    std::string mdlpath = fullpath.substr(0, fullpath.find_last_of("\\/"));
    std::string abs_path = mdlpath + mdlfilename;

    FILE *fp_mdl2 = fopen(abs_path.c_str(), "wb");
    fwrite(&df.header, sizeof(Data_disk_file_header), 1, fp_mdl2);
    fwrite(&dm.header, sizeof(Data_animation_header), 1, fp_mdl2);
    fwrite(&dm.bones[0], dm.bones.size() * sizeof(Data_animation_bone), 1, fp_mdl2);
    fwrite(&dm.frames[0], dm.frames.size() * sizeof(Data_animation_frame), 1, fp_mdl2);

    _filenames.push_back(abs_path);
    
    fclose(fp_mdl2);

}

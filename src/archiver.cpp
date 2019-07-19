#include "pch.h"
#include <engine/gfx_window.h>
#include <engine/gfx_shader.h>
#include <engine/gfx_sevbo.h>
#include <engine/gfx_primitives.h>
#include <engine/gfx_camera.h>
#include <engine/gfx_object.h>
#include <engine/gfx_texture.h>

#include <engine/data_archive.h>
#include <engine/data_file.h>

#include "win32/timer.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

int main(char** argv, int argc) {
/*
    {
        auto vbo1 = std::make_shared<SEVBO3D_pos_norm_uv>();

        Gfx_cube cube(glm::vec3(0, 0, 0), 1.0f);
        Gfx_object world_object;
        world_object.calc_bounds(cube.vertices);
        world_object.set_pos(glm::vec3(0, 0, 0));
        

        for (int i = 0; i < cube.vertices.size(); ++i) {
            auto v = cube.vertices[i];
            auto u = cube.uvs[i];
            auto n = cube.normals[i];

            vbo1->vertices.push_back(SEVBO_3D_pos_norm_uv_vertex(SEVBO_vertex(v.x, v.y, v.z), SEVBO_vertex(n.x, n.y, n.z), SEVBO_uv(u.x, u.y)));
        }

        for (auto i : cube.indices) {
            vbo1->indices.push_back(i);
        }

        vbo1->setup();
            
        Data_archive arc;
        
        auto file_tex = std::make_shared<Data_disk_file>();

        FILE *f = fopen("../data/asteroid_d.dds", "rb");

        fseek(f, 0L, SEEK_END);
        uint32_t tex_filesize = ftell(f);
        fseek(f, 0L, SEEK_SET);

        uint32_t tex_hash = Data_utils::generate_hash("asteroids_d.dds");

        Data_texture tex;
        tex.header.hash = tex_hash;
        tex.header.datasize = tex_filesize;
        tex.data = (uint8_t*)malloc(tex_filesize);
        fread(&tex.data[0], tex_filesize, 1, f);
        fclose(f);

        file_tex->header.filesize = sizeof(Data_disk_file_header) + sizeof(Data_texture_header) + tex_filesize;
        file_tex->header.filetype = Data_filetype::DDS;
        memset(file_tex->header.filename, 0, DATAFILE_FILENAME_MAX_LENGTH);
        strcpy(file_tex->header.filename, "asteroids_d.dds");
        file_tex->data = &tex;

        auto file_vbo = std::make_shared<Data_disk_file>();
        file_vbo->header.filetype = Data_filetype::MODEL;
        memset(file_vbo->header.filename, 0, DATAFILE_FILENAME_MAX_LENGTH);
        strcpy(file_vbo->header.filename, "asteroid.mdl");

        Data_model mdl;
        memcpy(&mdl.header.bb, &world_object.local_bounds[0], sizeof(float) * 8 * 3);
        //mdl.diffuse_id = tex_hash;
        mdl.header.emissive_id = 0;
        mdl.header.spec_id = 0;
        mdl.header.indices_data_size = vbo1->indice_buffer_size;
        mdl.header.vertices_data_size = vbo1->vertex_buffer_size;
        mdl.header.rendertype = static_cast<uint32_t>(Gfx_RT_type::RT_3D_POS_NORM_UV);

        uint32_t vbo_size = vbo1->vertex_buffer_size;

        mdl.verticesdata = vbo1->vert_data;
        mdl.indicedata = vbo1->indice_data;
        
        file_vbo->header.filesize = sizeof(Data_disk_file_header) + sizeof(Data_model_header) + mdl.header.vertices_data_size + mdl.header.indices_data_size;
        file_vbo->data = &mdl;

        arc.add_subfile(file_tex);
        arc.add_subfile(file_vbo);

        arc.write("../data/space.arc");

    }
    //std::vector<Data_toc_entry> toc;
    //Data_archive::read_toc("../data/space.arc", toc);

    Data_archive arc2;
    arc2.open("../data/space.arc");
    */
    return 0;
}
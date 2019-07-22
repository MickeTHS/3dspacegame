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
#include <engine/gfx_log.h>
#include "win32/timer.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

int main(char** argv, int argc) {

    Gfx_window win;
    win.init();

    //auto pos_norm_uv = Gfx_shader_store::get_instance().get_shader("pos_norm_uv");
    //assert(pos_norm_uv);
    auto pos_norm_uv_bone = Gfx_shader_store::get_instance().get_shader("pos_norm_uv_bone");
    assert(pos_norm_uv_bone);

    Gfx_timer time;
    
    Data_archive arc;
    arc.open("D:\\projects\\3dspacegame\\offline_assets\\pkg\\piston.arc");
    auto gfx_tex = arc.load_texture_file("piston_darkblue_d.ddx");
    auto gfx_obj = arc.load_model_file("piston.mdl");
    auto gfx_skel = arc.load_skeleton_file("piston_skeleton.skel");
    auto gfx_anim = arc.load_animation_file("piston_activate.anim");
    gfx_obj->set_skeleton(gfx_skel);

    glm::mat4 m4_model;
    glm::mat4 m4_mvp;
    time.init();
    
    pos_norm_uv_bone->v3_light0pos = glm::vec3(-3.0, 0.0, 0.0);

    auto cb = [&](){
        Gfx_log::Gfx_GL_debug(Gfx_log::MAIN_LOOP, "main test loop");

        time.frame();
        float delta = time.get_time();

        m4_model = glm::translate(m4_model, glm::vec3(0.0f, 0.0f, 0.0f));
        m4_model = glm::rotate(m4_model, delta * 0.001f, glm::vec3(0.7f, 1.0f, 0.5f));

        m4_mvp = win.get_camera().get_proj() * win.get_camera().get_view() * m4_model;
        pos_norm_uv_bone->m4_model = m4_model;
        pos_norm_uv_bone->m4_mvp = m4_mvp;
        pos_norm_uv_bone->m4_model_view = win.get_camera().get_view() * m4_model;
        
        // bind skeleton
        gfx_skel->calc_matrix_palette_bind(pos_norm_uv_bone->m4v_pal);
        gfx_tex->bind(pos_norm_uv_bone);

        pos_norm_uv_bone->activate();
        
        
        
        if (gfx_obj->intersect(win.get_camera().get_pos(), win.get_camera().get_lookat())) {
            gfx_obj->set_flag(Gfx_obj_flags::highlight, true);
            pos_norm_uv_bone->v4_highlight_color = glm::vec4(1,1,1,1);
        }
        else {
            gfx_obj->set_flag(Gfx_obj_flags::highlight, false);
            pos_norm_uv_bone->v4_highlight_color = glm::vec4(0.5, 0.5, 0.5, 1);
        }

        gfx_obj->get_draw_obj()->draw();
    };

    win.run(cb);

    return 0;
}
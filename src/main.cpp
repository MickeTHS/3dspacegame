#include "pch.h"
#include <engine/gfx_window.h>
#include <engine/gfx_shader.h>
#include <engine/gfx_sevbo.h>
#include <engine/gfx_primitives.h>
#include <engine/gfx_camera.h>
#include <engine/gfx_object.h>
#include <engine/gfx_texture.h>
#include <engine/gfx_animation.h>
#include <engine/gfx_skysphere.h>

#include <engine/data_archive.h>
#include <engine/data_file.h>
#include <engine/gfx_log.h>
#include "win32/timer.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

int main(char** argv, int argc) {

    std::shared_ptr<Gfx_timer> time = std::make_shared<Gfx_timer>();
    Gfx_window win;
    win.init(time);

    //auto pos_norm_uv = Gfx_shader_store::get_instance().get_shader("pos_norm_uv");
    //assert(pos_norm_uv);
    auto pos_norm_uv_bone = Gfx_shader_store::get_instance().get_shader("pos_norm_uv_bone");
    assert(pos_norm_uv_bone);

    std::shared_ptr<Gfx_animator> animator = std::make_shared<Gfx_animator>();
    
    Data_archive arc;
    arc.open("D:\\projects\\3dspacegame\\offline_assets\\pkg\\piston.arc");
    auto gfx_tex = arc.load_texture_file("piston_darkblue_d.ddx");
    auto gfx_obj = arc.load_model_file("piston.mdl");
    auto gfx_skel = arc.load_skeleton_file("piston_skeleton.skel");
    auto gfx_anim = arc.load_animation_file("piston_activate.anim");
    animator->add_animation_def(gfx_anim);

    gfx_obj->set_skeleton(gfx_skel);
    gfx_obj->set_animator(animator);
    gfx_obj->set_animation("piston_activate.anim");
    
    glm::mat4 m4_model;
    glm::mat4 m4_mvp;
    time->init();
    
    pos_norm_uv_bone->v3_light0pos = glm::vec3(-3.0, 0.0, 0.0);

    std::shared_ptr<Gfx_camera> camera = win.get_camera();
    
    Gfx_skysphere skysphere(camera);

    auto cb = [&](){
        Gfx_log::Gfx_GL_debug(Gfx_log::MAIN_LOOP, "main test loop");

        time->frame();
        float delta = time->get_time();



        //m4_model = glm::translate(m4_model, glm::vec3(0.0f, 0.0f, 0.0f));
        //m4_model = glm::rotate(m4_model, delta * 0.001f, glm::vec3(0.7f, 1.0f, 0.5f));
        //win.get_camera().update();

        m4_mvp = camera->get_proj() * camera->get_view() * m4_model;
        
        skysphere.draw(delta);
        
        pos_norm_uv_bone->m4_model = m4_model;
        pos_norm_uv_bone->m4_mvp = m4_mvp;
        pos_norm_uv_bone->m4_model_view = camera->get_view() * m4_model;
        
        
        gfx_tex->bind(pos_norm_uv_bone);
        auto animstate = gfx_obj->get_animation_state();

        if (animstate != nullptr) {
            //gfx_skel->calc_matrix_palette_bind(pos_norm_uv_bone->m4v_pal);
            animator->update(delta, pos_norm_uv_bone->m4v_pal);
            pos_norm_uv_bone->m4v_pal = animstate->palette;
        }
        else {
            // bind skeleton
            //gfx_skel->calc_matrix_palette_bind(pos_norm_uv_bone->m4v_pal);
        }

        pos_norm_uv_bone->activate();
        
        
        if (gfx_obj->intersect(camera->get_pos(), camera->get_lookat())) {
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
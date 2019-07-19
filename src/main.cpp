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

    Gfx_window win;
    win.init();

    auto posnormuv = Gfx_shader_store::get_instance().get_shader("posnormuv");
    assert(posnormuv);
    
    Gfx_timer time;
    
    Data_archive arc;
    arc.open("../data/space.arc");
    auto gfx_tex = arc.load_texture_file("asteroids_d.dds");
    auto gfx_obj = arc.load_model_file("asteroid.mdl");

    glm::mat4 m4_model;
    glm::mat4 m4_mvp;
    time.init();
    
    posnormuv->v3_light0pos = glm::vec3(-3.0, 0.0, 0.0);

    auto cb = [&](){
        time.frame();
        float delta = time.get_time();

        m4_model = glm::translate(m4_model, glm::vec3(0.0f, 0.0f, 0.0f));
        m4_model = glm::rotate(m4_model, delta * 0.001f, glm::vec3(0.7f, 1.0f, 0.5f));

        m4_mvp = win.get_camera().get_proj() * win.get_camera().get_view() * m4_model;
        posnormuv->m4_model = m4_model;
        posnormuv->m4_mvp = m4_mvp;
        posnormuv->activate();
        gfx_tex->bind(posnormuv);
        
        if (gfx_obj->intersect(win.get_camera().get_pos(), win.get_camera().get_lookat())) {
            gfx_obj->set_flag(Gfx_obj_flags::highlight, true);
            posnormuv->v4_highlight_color = glm::vec4(1,1,1,1);
        }
        else {
            gfx_obj->set_flag(Gfx_obj_flags::highlight, false);
            posnormuv->v4_highlight_color = glm::vec4(0.5, 0.5, 0.5, 1);
        }

        gfx_obj->get_draw_obj()->draw();
    };

    win.run(cb);

    return 0;
}
#include "engine/gfx_skysphere.h"
#include <engine/gfx_sevbo.h>

#include <glm/gtx/perpendicular.hpp>

void random_sphere_point(float& x, float& y, float& z, float radius) {
    float u = float(rand() % 1000) / 1000.0f;
    float v = float(rand() % 1000) / 1000.0f;
    float theta = 2.0f * 3.14159265359f * u;
    float phi = acosf(2.0f * v - 1);
    x = (radius * sinf(phi) * cosf(theta));
    y = (radius * sinf(phi) * sinf(theta));
    z = (radius * cosf(phi));
}

glm::vec2 perp_cw(glm::vec2& v) {
    return glm::vec2(v.y, -v.x);
}

glm::vec2 perp_ccw(glm::vec2& v) {
    return glm::vec2(-v.y, v.x);
}


Gfx_skysphere::Gfx_skysphere(std::shared_ptr<Gfx_camera> camera) {
    _shader = Gfx_shader_store::get_instance().get_shader("pos_uv");
    assert(_shader);

    _camera = camera;

    uint32_t num_stars = 512;

    std::vector<uint32_t> ratios = { uint32_t( num_stars * 0.60 ), uint32_t ( num_stars * 0.20 ) , uint32_t (num_stars * 0.15), uint32_t ( num_stars * 0.05 ) };

    std::vector<std::vector<float>> uvs = { 
        // smallest stars on row 0
    {
        0.0f, 0.0f, 0.25f, 0.25f,
        0.25f, 0.0f, 0.50f, 0.25f,
        0.50f, 0.0f, 0.75f, 0.25f,
    },
    {
        0.0f, 0.25f, 0.25f, 0.50f,
        0.25f, 0.25f, 0.50f, 0.50f,
        0.50f, 0.25f, 0.75f, 0.50f,
    },
    {
        0.0f, 0.50f, 0.25f, 0.75f,        
    },
    {
        0.0f, 0.75f, 0.25f, 1.0f,
    }
    };

    int s = 0;
    int count = 0;

    float star_radius = 0.1f;

    std::shared_ptr<SEVBO3D_pos_uv> vbo = std::make_shared<SEVBO3D_pos_uv>();

    glm::vec3 p0(-0.1f, -0.1f, 0.0f);
    glm::vec3 p1(+0.1f, -0.1f, 0.0f);
    glm::vec3 p2(+0.1f, +0.1f, 0.0f);
    glm::vec3 p3(-0.1f, +0.1f, 0.0f);

    
    for (int i = 0; i < num_stars; ++i) {
        if (s >= uvs.size()) {
            break;
        }

        int r = rand() % (uvs[s].size() / (4));

        assert(r >= 0);
        
        glm::vec3 v_c;
        
        random_sphere_point(v_c.x, v_c.y, v_c.z, 50.0f);

        float quadsize = 0.01f;

        camera->set_lookat(v_c);
        glm::mat4 view = camera->get_view();

        glm::vec3 cam_right = glm::vec3(view[0][0], view[1][0], view[2][0]);
        glm::vec3 cam_up = glm::vec3(view[0][1], view[1][1], view[2][1]);


        glm::vec3 particleCenter_wordspace = v_c;


        glm::vec3 v0 =
            particleCenter_wordspace
            + cam_right * p0.x
            + cam_up * p0.y;

        glm::vec3 v1 =
            particleCenter_wordspace
            + cam_right * p1.x
            + cam_up * p1.y;

        glm::vec3 v2 =
            particleCenter_wordspace
            + cam_right * p2.x
            + cam_up * p2.y;

        glm::vec3 v3 =
            particleCenter_wordspace
            + cam_right * p3.x
            + cam_up * p3.y;


        int uv_start = r * 3;

        SEVBO_uv top_left(uvs[s][uv_start], uvs[s][uv_start+1]);
        SEVBO_uv bottom_right(uvs[s][uv_start+2], uvs[s][uv_start + 3]);
        
        SEVBO_3D_pos_uv_vertex vert0;

        vert0.pos.x = v0.x;
        vert0.pos.y = v0.y;
        vert0.pos.z = v0.z;

        SEVBO_3D_pos_uv_vertex vert1;

        vert1.pos.x = v1.x;
        vert1.pos.y = v1.y;
        vert1.pos.z = v1.z;

        SEVBO_3D_pos_uv_vertex vert2;

        vert2.pos.x = v2.x;
        vert2.pos.y = v2.y;
        vert2.pos.z = v2.z;

        SEVBO_3D_pos_uv_vertex vert3;

        vert3.pos.x = v3.x;
        vert3.pos.y = v3.y;
        vert3.pos.z = v3.z;

        vbo->vertices.push_back(vert0);
        vbo->vertices.push_back(vert1);
        vbo->vertices.push_back(vert2);
        vbo->vertices.push_back(vert3);

        vbo->indices.push_back((i * 4) + 0);
        vbo->indices.push_back((i * 4) + 1);
        vbo->indices.push_back((i * 4) + 2);

        vbo->indices.push_back((i * 4) + 2);
        vbo->indices.push_back((i * 4) + 3);
        vbo->indices.push_back((i * 4) + 0);

        if (ratios[s] < count++) {
            // step to next row
            count = 0;
            s++;

            if (s == ratios.size()) {
                break;
            }
        }
    }

    vbo->setup();

    _draw_obj = std::make_shared<Gfx_draw_object>();
    _draw_obj->setup(vbo, _shader);
    
}

Gfx_skysphere::~Gfx_skysphere() {

}

void Gfx_skysphere::draw(float delta) {
    _shader->m4_proj = _camera->get_proj();
    _shader->m4_view = _camera->get_view();
    glm::vec3 p(0, 0, 0);

    _shader->m4_model = glm::translate(glm::mat4(1.0f), p);
    //_shader->m4_model_view = _camera->get_view() * _shader->m4_model;
    //_shader->m4_rotation = glm::mat4(1.0);

    

    glm::mat4 proj = _camera->get_proj();
    glm::mat4 view = _camera->get_view();
    
    _shader->m4_mvp = proj * view * _shader->m4_model;
    _shader->m4_proj_view = proj * view;

    _shader->activate();

    _draw_obj->draw();
}
#pragma once

#include "pch.h"
#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>

struct Gfx_triangle {};

struct Gfx_quad {
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    Gfx_quad() {}

    Gfx_quad(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3) {
        vertices.push_back(p0);
        vertices.push_back(p1);
        vertices.push_back(p2);
        vertices.push_back(p3);

        uvs.push_back(glm::vec2(0, 0));
        uvs.push_back(glm::vec2(1, 0));
        uvs.push_back(glm::vec2(1, 1));
        uvs.push_back(glm::vec2(0, 1));
                
        glm::vec3 n0 = glm::triangleNormal(p0, p1, p2);
        glm::vec3 n1 = glm::triangleNormal(p2, p3, p0);

        normals.push_back(n0);
        normals.push_back(n0);
        normals.push_back(n0);
        normals.push_back(n0);

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);

        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(0);
    }

    Gfx_quad(glm::vec3& pos, float size) {
        vertices.push_back(glm::vec3(pos.x - size / 2.0, pos.y - size / 2.0, pos.z));
        vertices.push_back(glm::vec3(pos.x + size / 2.0, pos.y - size / 2.0, pos.z));
        vertices.push_back(glm::vec3(pos.x + size / 2.0, pos.y + size / 2.0, pos.z));
        vertices.push_back(glm::vec3(pos.x - size / 2.0, pos.y + size / 2.0, pos.z));

        uvs.push_back(glm::vec2(0, 0));
        uvs.push_back(glm::vec2(1, 0));
        uvs.push_back(glm::vec2(1, 1));
        uvs.push_back(glm::vec2(0, 1));

        glm::vec3 n0 = glm::triangleNormal(vertices[0], vertices[1], vertices[2]);
        glm::vec3 n1 = glm::triangleNormal(vertices[2], vertices[3], vertices[0]);

        normals.push_back(n0);
        normals.push_back(n0);
        normals.push_back(n0);
        normals.push_back(n0);


        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);

        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(0);
    }

    void copy_to_sevbo_pos_col(std::vector<SEVBO_3D_pos_col_vertex>& dst, SEVBO_vertex color) {
        for (auto v : vertices) {
            dst.push_back(SEVBO_3D_pos_col_vertex(SEVBO_vertex(v.x, v.y, v.z), color));
        }
    }
};

struct Gfx_cube {
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    Gfx_cube(glm::vec3& pos, float size) {
        glm::vec3 v0(pos.x - size / 2.0, pos.y - size / 2.0, pos.z - size / 2.0);
        glm::vec3 v1(pos.x + size / 2.0, pos.y - size / 2.0, pos.z - size / 2.0);
        glm::vec3 v2(pos.x + size / 2.0, pos.y + size / 2.0, pos.z - size / 2.0);
        glm::vec3 v3(pos.x - size / 2.0, pos.y + size / 2.0, pos.z - size / 2.0);
        
        glm::vec3 v4(pos.x - size / 2.0, pos.y - size / 2.0, pos.z + size / 2.0);
        glm::vec3 v5(pos.x + size / 2.0, pos.y - size / 2.0, pos.z + size / 2.0);
        glm::vec3 v6(pos.x + size / 2.0, pos.y + size / 2.0, pos.z + size / 2.0);
        glm::vec3 v7(pos.x - size / 2.0, pos.y + size / 2.0, pos.z + size / 2.0);


        // front
        Gfx_quad p0(v0, v1, v2, v3);

        // back
        Gfx_quad p1(v4, v5, v6, v7);
        
        // left
        Gfx_quad p2(v0, v3, v7, v4);

        // right
        Gfx_quad p3(v1, v5, v6, v2);

        // top
        Gfx_quad p4(v0, v4, v5, v1);
        
        // bottom 
        Gfx_quad p5(v3, v2, v6, v7);

        vertices.resize(4 * 6);
        normals.resize(4 * 6);
        uvs.resize(4 * 6);

        memcpy(&vertices[0], &p0.vertices[0], sizeof(glm::vec3) * 4);
        memcpy(&vertices[4], &p1.vertices[0], sizeof(glm::vec3) * 4);
        memcpy(&vertices[8], &p2.vertices[0], sizeof(glm::vec3) * 4);
        memcpy(&vertices[12], &p3.vertices[0], sizeof(glm::vec3) * 4);
        memcpy(&vertices[16], &p4.vertices[0], sizeof(glm::vec3) * 4);
        memcpy(&vertices[20], &p5.vertices[0], sizeof(glm::vec3) * 4);

        memcpy(&normals[0], &p0.normals[0], sizeof(glm::vec3) * 4);
        memcpy(&normals[4], &p1.normals[0], sizeof(glm::vec3) * 4);
        memcpy(&normals[8], &p2.normals[0], sizeof(glm::vec3) * 4);
        memcpy(&normals[12], &p3.normals[0], sizeof(glm::vec3) * 4);
        memcpy(&normals[16], &p4.normals[0], sizeof(glm::vec3) * 4);
        memcpy(&normals[20], &p5.normals[0], sizeof(glm::vec3) * 4);

        memcpy(&uvs[0], &p0.uvs[0], sizeof(glm::vec2) * 4);
        memcpy(&uvs[4], &p1.uvs[0], sizeof(glm::vec2) * 4);
        memcpy(&uvs[8], &p2.uvs[0], sizeof(glm::vec2) * 4);
        memcpy(&uvs[12], &p3.uvs[0], sizeof(glm::vec2) * 4);
        memcpy(&uvs[16], &p4.uvs[0], sizeof(glm::vec2) * 4);
        memcpy(&uvs[20], &p5.uvs[0], sizeof(glm::vec2) * 4);


        for (int i = 0; i < 6; ++i) {
            indices.push_back((i * 4) + 0); //4
            indices.push_back((i * 4) + 1); //5
            indices.push_back((i * 4) + 2); //6

            indices.push_back((i * 4) + 2); //6
            indices.push_back((i * 4) + 3); //7
            indices.push_back((i * 4) + 0); //4
        }

    }
};


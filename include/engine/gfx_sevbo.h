#pragma once

#include "pch.h"

#include <GL/glew.h>
#include <engine/gfx_shader.h>


#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define NumberOf(array)        (sizeof(array)/sizeof(array[0]))

struct SEVBO_header {
    char filename[32];
    uint32_t vert_size;
    uint32_t index_size;
};


struct SEVBO_vertex {
    float x;
    float y;
    float z;

    SEVBO_vertex() {}

    SEVBO_vertex(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {

    }


    void print() {
        printf("  pos: %f, %f, %f", x, y, z);
    }
};

struct SEVBO_uv {
    float u;
    float v;

    SEVBO_uv() {}
    SEVBO_uv(float u_, float v_) : u(u_), v(v_) {}

    void print() {
        printf("  uv: %f, %f, %f", u, v);
    }
};


struct SEVBO_3D_pos_norm_uv_vertex {
    SEVBO_vertex pos;
    SEVBO_vertex norm;
    SEVBO_uv uv;

    SEVBO_3D_pos_norm_uv_vertex() {}

    SEVBO_3D_pos_norm_uv_vertex(SEVBO_vertex& p, SEVBO_vertex& n, SEVBO_uv& u) {
        memcpy(&pos, &p, sizeof(SEVBO_vertex));
        memcpy(&norm, &n, sizeof(SEVBO_vertex));
        memcpy(&uv, &u, sizeof(SEVBO_uv));
    }
};


struct SEVBO_3D_pos_uv_vertex {
    SEVBO_vertex pos;
    SEVBO_uv uv;

    SEVBO_3D_pos_uv_vertex() {}

    SEVBO_3D_pos_uv_vertex(SEVBO_vertex& p, SEVBO_uv& u) {
        memcpy(&pos, &p, sizeof(SEVBO_vertex));
        memcpy(&uv, &u, sizeof(SEVBO_uv));
    }
};

struct SEVBO_3D_pos_col_vertex {
    SEVBO_vertex pos;
    SEVBO_vertex col;

    SEVBO_3D_pos_col_vertex() {}

    SEVBO_3D_pos_col_vertex(SEVBO_vertex& p, SEVBO_vertex& c) {
        memcpy(&pos, &p, sizeof(SEVBO_vertex));
        memcpy(&col, &c, sizeof(SEVBO_vertex));
    }

    void print() {
        printf("vertex: \n");
        pos.print();
        col.print();
    }
};

struct SEVBO_2D_vertex {
    SEVBO_vertex pos;
    SEVBO_uv	 uv;


    void print() {
        printf("vertex: \n");
        pos.print();
        uv.print();
    }
};


struct SEVBO {
    SEVBO_header header;

    uint32_t              vertex_size;
    uint32_t              vertex_buffer_size;
    uint32_t              indice_buffer_size;

    std::vector<uint32_t> offsets;
    
    uint32_t              num_indices;
    uint32_t              num_vertices;

    void* vert_data;
    void* indice_data;

    //virtual bool setup() = 0;

    static uint32_t typesize(uint32_t type) {
        switch(type) {
            case RT_3D_POS_NORM_UV: return sizeof(SEVBO_3D_pos_norm_uv_vertex);
            case RT_3D_POS_UV: return sizeof(SEVBO_3D_pos_uv_vertex);
            default: return 0;
        }
    }
};


struct SEVBO3D_pos_norm_uv : SEVBO {
    std::vector<SEVBO_3D_pos_norm_uv_vertex> vertices;
    std::vector<uint32_t> indices;

    SEVBO3D_pos_norm_uv() {
        vertex_size = sizeof(SEVBO_3D_pos_norm_uv_vertex);
        offsets.push_back(sizeof(SEVBO_vertex));
        offsets.push_back(sizeof(SEVBO_vertex) + sizeof(SEVBO_vertex));
        offsets.push_back(sizeof(SEVBO_vertex) + sizeof(SEVBO_vertex) + sizeof(SEVBO_uv));
    }

    virtual bool setup() {
        num_vertices = vertices.size();
        num_indices = indices.size();
        header.index_size = num_indices * sizeof(uint32_t);
        header.vert_size = num_vertices * vertex_size;

        vertex_buffer_size = sizeof(SEVBO_3D_pos_norm_uv_vertex) * num_vertices;
        indice_buffer_size = sizeof(uint32_t) * num_indices;

        vert_data = vertices.data();
        indice_data = indices.data();

        if (num_vertices <= 0) {
            return false;
        }

        return true;
    }
};



struct SEVBO3D_pos_uv : SEVBO {
    std::vector<SEVBO_3D_pos_uv_vertex> vertices;
    std::vector<uint32_t> indices;

    SEVBO3D_pos_uv() {
        vertex_size = sizeof(SEVBO_3D_pos_uv_vertex);
        offsets.push_back(sizeof(SEVBO_vertex));
        offsets.push_back(sizeof(SEVBO_vertex) + sizeof(SEVBO_vertex));
    }

    virtual bool setup() {
        num_vertices = vertices.size();
        num_indices = indices.size();
        header.index_size = num_indices * sizeof(uint32_t);
        header.vert_size = num_vertices * vertex_size;

        vertex_buffer_size = sizeof(SEVBO_3D_pos_uv_vertex) * num_vertices;
        indice_buffer_size = sizeof(uint32_t) * num_indices;

        vert_data = vertices.data();
        indice_data = indices.data();

        if (num_vertices <= 0) {
            return false;
        }

        return true;
    }
};

struct SEVBO3D_pos_col : SEVBO {
    std::vector<SEVBO_3D_pos_col_vertex> vertices;
    std::vector<uint32_t> indices;

    SEVBO3D_pos_col() {
        vertex_size = sizeof(SEVBO_3D_pos_col_vertex);
        offsets.push_back(sizeof(SEVBO_vertex));
        offsets.push_back(sizeof(SEVBO_vertex) + sizeof(SEVBO_vertex));
    }

    virtual bool setup() {
        num_vertices = vertices.size();
        num_indices = indices.size();
        header.index_size = num_indices * sizeof(uint32_t);
        header.vert_size = num_vertices * vertex_size;

        vertex_buffer_size = sizeof(SEVBO_3D_pos_col_vertex) * num_vertices;
        indice_buffer_size = sizeof(uint32_t) * num_indices;

        vert_data = vertices.data();
        indice_data = indices.data();

        if (num_vertices <= 0) {
            return false;
        }

        return true;
    }
};

struct SEVBO_2D_pos_uv : SEVBO {
    std::vector<SEVBO_2D_vertex> vertices;
    std::vector<uint32_t> indices;

    SEVBO_2D_pos_uv() {
        vertex_size = sizeof(SEVBO_2D_vertex);
        offsets.push_back(sizeof(SEVBO_vertex));
        offsets.push_back(sizeof(SEVBO_vertex) + sizeof(SEVBO_uv));
    }

    virtual bool setup() {
        num_vertices = vertices.size();
        num_indices = indices.size();

        vert_data = vertices.data();
        indice_data = indices.data();

        if (num_vertices <= 0) {
            return false;
        }

        return true;
    }
};


struct Gfx_draw_object {
	uint32_t vertexArrayObjID;
	uint32_t programId;
	uint32_t vertexBufferId;
	uint32_t indexBufferId;
	uint32_t numElements;
	std::string name;
	std::shared_ptr<Gfx_shader> shader;

	void setup(std::shared_ptr<SEVBO> object_data, std::shared_ptr<Gfx_shader> shader) {
		this->shader = shader;
	// ifdef OPENGL
		name = std::string(object_data->header.filename);

		glGenVertexArrays(1, &vertexArrayObjID);
		glBindVertexArray(vertexArrayObjID);

        glGenBuffers(1, &vertexBufferId);
        glGenBuffers(1, &indexBufferId);

        shader->bindVBO(object_data, vertexBufferId, indexBufferId);

		glBindVertexArray(0);

		numElements = object_data->num_indices;
	}
	

	void draw() {
    	// ifdef OPENGL
		glBindVertexArray(vertexArrayObjID);	// First VAO
												// To render, we can either use glDrawElements or glDrawRangeElements
												// The is the number of indices. 3 indices needed to make a single triangle
		glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, BUFFER_OFFSET(0));    // The starting poi
		glBindVertexArray(0);	// First VAO

	}

	void gpuDelete() {
		// ifdef OPENGL
		// Cleanup VBO and shader
		glDeleteBuffers(1, &vertexBufferId);
		glDeleteBuffers(1, &indexBufferId);
		glDeleteVertexArrays(1, &vertexArrayObjID);
	}
};


uint16_t loadSEVBO(const char * filepath, std::vector<SEVBO*>& vbos);

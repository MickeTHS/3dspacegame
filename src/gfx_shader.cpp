#include "pch.h"
#include <GL/glew.h>

#include "engine/gfx_shader.h"
#include "engine/gfx_sevbo.h"

bool Gfx_shader_store::init() {
    
    auto pos_col = std::make_shared<Gfx_RT_3D_pos_col>();
    pos_col->setup();
    auto pos_uv = std::make_shared<Gfx_RT_3D_pos_uv>();
    pos_uv->setup();
    auto pos_norm_uv = std::make_shared<Gfx_RT_3D_pos_norm_uv>();
    pos_norm_uv->setup();
    auto pos_norm_uv_bone = std::make_shared<Gfx_RT_3D_pos_norm_uv_bone>();
    pos_norm_uv_bone->setup();

    add_shader(pos_col);
    add_shader(pos_norm_uv);
    add_shader(pos_uv);
    add_shader(pos_norm_uv_bone);

    return true;
}

GLint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

    // Create the shaders
    GLint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    GLint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vs_path = SHADER_PATH + std::string(vertex_file_path);
    std::string fs_path = SHADER_PATH + std::string(fragment_file_path);


    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vs_path, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vs_path.c_str());
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fs_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;


    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(vertex_shader_id, 1, &VertexSourcePointer, NULL);
    glCompileShader(vertex_shader_id);

    // Check Vertex Shader
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(vertex_shader_id, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }



    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(fragment_shader_id, 1, &FragmentSourcePointer, NULL);
    glCompileShader(fragment_shader_id);

    // Check Fragment Shader
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(fragment_shader_id, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }



    // Link the program
    printf("Linking program\n");
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    // Check the program
    glGetProgramiv(program_id, GL_LINK_STATUS, &Result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(program_id, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }


    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return program_id;
}


bool Gfx_RT_3D_pos_col::bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId) {
    
    uint32_t pos = glGetAttribLocation(program_id, "pos");
    uint32_t col = glGetAttribLocation(program_id, "col");

    glEnableVertexAttribArray(pos);
    glEnableVertexAttribArray(col);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SEVBO_3D_pos_col_vertex) * vbo->num_vertices, vbo->vert_data, GL_STATIC_DRAW);

    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, sizeof(SEVBO_3D_pos_col_vertex), BUFFER_OFFSET(0));
    glVertexAttribPointer(col, 3, GL_FLOAT, GL_FALSE, sizeof(SEVBO_3D_pos_col_vertex), BUFFER_OFFSET(sizeof(SEVBO_vertex)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * vbo->num_indices, vbo->indice_data, GL_STATIC_DRAW);

    glBindVertexArray(0);

    return true;
}


bool Gfx_RT_3D_pos_norm_uv_bone::bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId) {

    uint32_t pos = glGetAttribLocation(program_id, "v_pos");
    uint32_t norm = glGetAttribLocation(program_id, "v_norm");
    uint32_t uv = glGetAttribLocation(program_id, "v_uv");
    uint32_t bone = glGetAttribLocation(program_id, "f_bone");

    glEnableVertexAttribArray(pos);
    glEnableVertexAttribArray(norm);
    glEnableVertexAttribArray(uv);
    glEnableVertexAttribArray(bone);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SEVBO_3D_pos_norm_uv_bone_vertex) * vbo->num_vertices, vbo->vert_data, GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);    // We like submitting vertices on stream 0 for no special reason
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, sizeof(SEVBO_3D_pos_norm_uv_bone_vertex), BUFFER_OFFSET(0));
    glVertexAttribPointer(norm, 3, GL_FLOAT, GL_FALSE, sizeof(SEVBO_3D_pos_norm_uv_bone_vertex), BUFFER_OFFSET(sizeof(SEVBO_vertex)));
    glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, sizeof(SEVBO_3D_pos_norm_uv_bone_vertex), BUFFER_OFFSET(sizeof(SEVBO_vertex) + sizeof(SEVBO_vertex)));
    glVertexAttribPointer(bone, 1, GL_INT, GL_FALSE, sizeof(SEVBO_3D_pos_norm_uv_bone_vertex), BUFFER_OFFSET(sizeof(SEVBO_vertex) + sizeof(SEVBO_vertex) + sizeof(SEVBO_uv)));

    
    std::vector<SEVBO_3D_pos_norm_uv_bone_vertex> test;
    test.resize(vbo->num_vertices);
    memcpy(&test[0], &((uint8_t*)vbo->vert_data)[0], sizeof(SEVBO_3D_pos_norm_uv_bone_vertex) * vbo->num_vertices);
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * vbo->num_indices, vbo->indice_data, GL_STATIC_DRAW);

    /*std::vector<uint32_t> bones;
    bones.resize(vbo->num_indices);
    memcpy(&bones[0], &((uint8_t*)vbo->indice_data)[0], sizeof(uint32_t) * vbo->num_indices);
    */
    glBindVertexArray(0);

    return true;
}

bool Gfx_RT_3D_pos_norm_uv::bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId) {

    uint32_t pos = glGetAttribLocation(program_id, "pos");
    uint32_t norm = glGetAttribLocation(program_id, "norm");
    uint32_t uv = glGetAttribLocation(program_id, "uv");

    glEnableVertexAttribArray(pos);
    glEnableVertexAttribArray(norm);
    glEnableVertexAttribArray(uv);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SEVBO_3D_pos_norm_uv_vertex) * vbo->num_vertices, vbo->vert_data, GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);    // We like submitting vertices on stream 0 for no special reason
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, sizeof(SEVBO_3D_pos_norm_uv_vertex), BUFFER_OFFSET(0));
    glVertexAttribPointer(norm, 3, GL_FLOAT, GL_FALSE, sizeof(SEVBO_3D_pos_norm_uv_vertex), BUFFER_OFFSET(sizeof(SEVBO_vertex))); 
    glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, sizeof(SEVBO_3D_pos_norm_uv_vertex), BUFFER_OFFSET(sizeof(SEVBO_vertex) + sizeof(SEVBO_vertex))); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * vbo->num_indices, vbo->indice_data, GL_STATIC_DRAW);

    glBindVertexArray(0);

    return true;
}


bool Gfx_RT_3D_pos_uv::bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId) {

    uint32_t pos = glGetAttribLocation(program_id, "pos");
    uint32_t tex = glGetAttribLocation(program_id, "uv");

    glEnableVertexAttribArray(pos);
    glEnableVertexAttribArray(tex);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SEVBO_3D_pos_uv_vertex) * vbo->num_vertices, vbo->vert_data, GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);    // We like submitting vertices on stream 0 for no special reason
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, sizeof(SEVBO_3D_pos_uv_vertex), BUFFER_OFFSET(0));      // The starting point of the VBO, for the vertices
                                                                                                       //glEnableVertexAttribArray(1);    // We like submitting normals on stream 1 for no special reason
    glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, sizeof(SEVBO_3D_pos_uv_vertex), BUFFER_OFFSET(sizeof(SEVBO_vertex)));     // The starting point of normals, 12 bytes away

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * vbo->num_indices, vbo->indice_data, GL_STATIC_DRAW);

    glBindVertexArray(0);

    return true;
}

bool Gfx_RT_3D_pos_uv_alpha::bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId) {
    uint32_t pos = glGetAttribLocation(program_id, "pos");
    uint32_t tex = glGetAttribLocation(program_id, "uv");

    glEnableVertexAttribArray(pos);
    glEnableVertexAttribArray(tex);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, vbo->header.vert_size, vbo->vert_data, GL_STATIC_DRAW);
    //glEnableVertexAttribArray(0);    // We like submitting vertices on stream 0 for no special reason
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, sizeof(SEVBO_2D_vertex), BUFFER_OFFSET(0));      // The starting point of the VBO, for the vertices
                                                                                                       //glEnableVertexAttribArray(1);    // We like submitting normals on stream 1 for no special reason
    glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, sizeof(SEVBO_2D_vertex), BUFFER_OFFSET(sizeof(SEVBO_vertex)));     // The starting point of normals, 12 bytes away

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vbo->header.index_size, vbo->indice_data, GL_STATIC_DRAW);

    glBindVertexArray(0);


    return true;
}

bool Gfx_shader::setup(const char* name_, const char* vsfile, const char* fsfile) {
    char debugmsg[128];
    sprintf(debugmsg, "shader init: %s", name_);

    Gfx_log::Gfx_GL_debug(Gfx_log::SHADER_INIT, debugmsg);

    name = std::string(name_);

    program_id = LoadShaders(vsfile, fsfile);

    if (program_id < 0) {
        printf("Gfx_shader: failure to load shader: %s, %s\n", vsfile, fsfile);
        return false;
    }

    u_mvp = glGetUniformLocation(program_id, "mvp");
    u_model = glGetUniformLocation(program_id, "model");
    u_model_view = glGetUniformLocation(program_id, "model_view");
    u_tex0 = glGetUniformLocation(program_id, "tex0");
    u_highlight = glGetUniformLocation(program_id, "highlight");
    u_light0pos = glGetUniformLocation(program_id, "light0pos");
    u_pal = glGetUniformLocation(program_id, "pal");

    if (u_mvp < 0) {
        printf("Gfx_shader: Notice: Shader without mvp\n");
    }

    if (u_tex0 < 0) {
        printf("Gfx_shader: Notice: Shader without tex0\n");
    }


    return true;
}



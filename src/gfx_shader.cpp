#include "pch.h"
#include <GL/glew.h>

#include "engine/gfx_shader.h"
#include "engine/gfx_sevbo.h"

bool Gfx_shader_store::init() {
    
    auto poscol = std::make_shared<Gfx_RT_3D_pos_col>();
    poscol->setup();
    auto posuv = std::make_shared<Gfx_RT_3D_pos_uv>();
    posuv->setup();
    auto posnormuv = std::make_shared<Gfx_RT_3D_pos_norm_uv>();
    posnormuv->setup();

    add_shader(poscol);
    add_shader(posnormuv);
    add_shader(posuv);

    return true;
}

GLint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

    // Create the shaders
    GLint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

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
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }



    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }



    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }


    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}


bool Gfx_RT_3D_pos_col::bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId) {
    
    uint32_t pos = glGetAttribLocation(programId, "pos");
    uint32_t col = glGetAttribLocation(programId, "col");

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


bool Gfx_RT_3D_pos_norm_uv::bindVBO(std::shared_ptr<SEVBO> vbo, uint32_t vertexBufferId, uint32_t indexBufferId) {

    uint32_t pos = glGetAttribLocation(programId, "pos");
    uint32_t norm = glGetAttribLocation(programId, "norm");
    uint32_t uv = glGetAttribLocation(programId, "uv");

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

    uint32_t pos = glGetAttribLocation(programId, "pos");
    uint32_t tex = glGetAttribLocation(programId, "uv");

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
    uint32_t pos = glGetAttribLocation(programId, "pos");
    uint32_t tex = glGetAttribLocation(programId, "uv");

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
    name = std::string(name_);

    programId = LoadShaders(vsfile, fsfile);

    if (programId < 0) {
        printf("Gfx_shader: failure to load shader: %s, %s\n", vsfile, fsfile);
        return false;
    }

    u_mvp = glGetUniformLocation(programId, "mvp");
    u_model = glGetUniformLocation(programId, "model");
    u_tex0 = glGetUniformLocation(programId, "tex0");
    u_highlight = glGetUniformLocation(programId, "highlight");
    u_light0pos = glGetUniformLocation(programId, "light0pos");

    if (u_mvp < 0) {
        printf("Gfx_shader: Notice: Shader without mvp\n");
    }

    if (u_tex0 < 0) {
        printf("Gfx_shader: Notice: Shader without tex0\n");
    }


    return true;
}



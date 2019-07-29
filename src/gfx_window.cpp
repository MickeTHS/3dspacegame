#include "engine/gfx_window.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <engine/gfx_log.h>
#include <engine/gfx_primitives.h>

void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{

    char sev[50];

    switch(severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            sprintf(sev, "HIGH!");
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            sprintf(sev, "MEDIUM");
            break;
        case GL_DEBUG_SEVERITY_LOW:
            sprintf(sev, "LOW");
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return;
            sprintf(sev, "NOTIFICATION");
            break;
        default:
            sprintf(sev, "UNKNOWN");
    }
    
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = %s, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, sev, message);
}

Gfx_window::Gfx_window() : _window(NULL), _mouse_init(false), _yaw(0.0f), _pitch(0.0f), _timer(nullptr) {
    
}

Gfx_window::~Gfx_window() {
    if (_window != NULL) {
        //glfwDestroyWindow(_window);
        glfwTerminate();
    }
}

void Gfx_window::mouse_button_pressed(int button, int action, int mods) {
    
    if (action == GLFW_PRESS) {
        //printf("down\n");
    }
    else if (action == GLFW_RELEASE) {
        //printf("up\n");
    }

}


void Gfx_window::key_action(int key, int action, int mods) {
    bool press = action == GLFW_PRESS || action == GLFW_REPEAT;

    if (key == GLFW_KEY_W) {
        _camera.set_move_forward(press);
    }
    if (key == GLFW_KEY_A) {
        _camera.set_move_left(press);
    }
    if (key == GLFW_KEY_S) {
        _camera.set_move_backward(press);
    }
    if (key == GLFW_KEY_D) {
        _camera.set_move_right(press);
    }

}

float Gfx_window::yaw() { return _yaw; }

float Gfx_window::pitch() { return _pitch; }

Gfx_camera& Gfx_window::get_camera() {
    return _camera;
}

void Gfx_window::mouse_moved(double x, double y) {
    //printf("move: %f, %f\n", x, y);

    if (!_mouse_init) {
        _mouse_init = true;
        _mouse_pos = glm::vec2(x,y);
        return;
    }

    //always compute delta
    //mousePosition is the last mouse position
    glm::vec2 mouse_delta = glm::vec2(_width/2.0f, _height/2.0f) - glm::vec2(x, y);

    const float sensitivity_x = 0.025f;
    const float sensitivity_y = 0.025f;
    //note that yaw and pitch must be converted to radians.
    //this is done in UpdateView() by glm::rotate
    _yaw += sensitivity_x * mouse_delta.x;
    _pitch += sensitivity_y * mouse_delta.y;

    _mouse_pos = glm::vec2(x, y);

    _camera.update_direction(mouse_delta.x, mouse_delta.y);

    glfwSetCursorPos(_window, _width/2.0f, _height/2.0f);
}

bool Gfx_window::init(std::shared_ptr<Gfx_timer> timer) {
    _timer = timer;
    
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    

    uint32_t width = 1024, height = 768;

    _width = width;
    _height = height;

    // Open a window and create its OpenGL context
    _window = glfwCreateWindow(width, height, "3dgame", NULL, NULL);
    if (_window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(_window);
    
    _camera.init((float)width, (float)height);
    
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return false;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(_window, GLFW_STICKY_KEYS, GL_TRUE);

    // hide the cursor
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, 0);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    //glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    //glDepthFunc(GL_LESS);

    GLenum err = 0;
    while ((err = glGetError()) != GL_NO_ERROR) {
        printf("Gfx_window(1): OpenGL error: %d\n", err);
    }

    glfwSetWindowUserPointer(_window, this);

    auto keyFunc = [](GLFWwindow* w, int key, int scancode, int action, int mods)
    {
        static_cast<Gfx_window*>(glfwGetWindowUserPointer(w))->key_action(key, action, mods);
    };
    
    auto btnfunc = [](GLFWwindow* w, int button, int action, int mods)
    {
        static_cast<Gfx_window*>(glfwGetWindowUserPointer(w))->mouse_button_pressed(button, action, mods);
    };

    auto movefunc = [](GLFWwindow* w, double x, double y)
    {
        static_cast<Gfx_window*>(glfwGetWindowUserPointer(w))->mouse_moved(x, y);
    };

    glfwSetMouseButtonCallback(_window, btnfunc);
    glfwSetCursorPosCallback(_window, movefunc);
    glfwSetKeyCallback(_window, keyFunc);

    Gfx_shader_store::get_instance().init();
    _pos_col = Gfx_shader_store::get_instance().get_shader("pos_col");
    
    std::shared_ptr<Gfx_draw_object> crosshair = std::make_shared<Gfx_draw_object>();

    auto vbo1 = std::make_shared<SEVBO3D_pos_col>();

    float thickness = 0.002f;
    float length = 0.03f;
    float offset = 0.003f;

    Gfx_quad left(
        glm::vec3(-(offset + length), -thickness / 2.0f, 0), 
        glm::vec3(-(offset), -thickness / 2.0f, 0), 
        glm::vec3(-(offset), +thickness / 2.0f, 0),
        glm::vec3(-(offset + length), +thickness / 2.0f, 0)
    );

    Gfx_quad right(
        glm::vec3(+(offset + length), -thickness / 2.0f, 0),
        glm::vec3(+(offset), -thickness / 2.0f, 0),
        glm::vec3(+(offset), +thickness / 2.0f, 0),
        glm::vec3(+(offset + length), +thickness / 2.0f, 0)
    );

    Gfx_quad top(
        glm::vec3(-thickness / 2.0f, +(offset + length), 0),
        glm::vec3(-thickness / 2.0f, +(offset), 0),
        glm::vec3(+thickness / 2.0f, +(offset), 0),
        glm::vec3(+thickness / 2.0f, +(offset + length), 0)
    );

    Gfx_quad bottom(
        glm::vec3(-thickness / 2.0f, -(offset + length), 0),
        glm::vec3(-thickness / 2.0f, -(offset), 0),
        glm::vec3(+thickness / 2.0f, -(offset), 0),
        glm::vec3(+thickness / 2.0f, -(offset + length), 0)
    );

    SEVBO_vertex color(0.3f, 0.3f, 0.3f);

    left.copy_to_sevbo_pos_col(vbo1->vertices, color);
    right.copy_to_sevbo_pos_col(vbo1->vertices, color);
    top.copy_to_sevbo_pos_col(vbo1->vertices, color);
    bottom.copy_to_sevbo_pos_col(vbo1->vertices, color);

    vbo1->indices.push_back(0);
    vbo1->indices.push_back(1);
    vbo1->indices.push_back(2);

    vbo1->indices.push_back(2);
    vbo1->indices.push_back(3);
    vbo1->indices.push_back(0);

    vbo1->indices.push_back(4 + 0);
    vbo1->indices.push_back(4 + 1);
    vbo1->indices.push_back(4 + 2);

    vbo1->indices.push_back(4 + 2);
    vbo1->indices.push_back(4 + 3);
    vbo1->indices.push_back(4 + 0);

    vbo1->indices.push_back(8 + 0);
    vbo1->indices.push_back(8 + 1);
    vbo1->indices.push_back(8 + 2);

    vbo1->indices.push_back(8 + 2);
    vbo1->indices.push_back(8 + 3);
    vbo1->indices.push_back(8 + 0);

    vbo1->indices.push_back(12 + 0);
    vbo1->indices.push_back(12 + 1);
    vbo1->indices.push_back(12 + 2);

    vbo1->indices.push_back(12 + 2);
    vbo1->indices.push_back(12 + 3);
    vbo1->indices.push_back(12 + 0);


    vbo1->setup();

    _crosshair = std::make_shared<Gfx_draw_object>();
    _crosshair->setup(vbo1, _pos_col);
    
    return true;

}

bool Gfx_window::run(std::function<void()> drawcall) {
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);


    while(true) {
        _camera.update(_timer->get_time());

        Gfx_log::Gfx_GL_debug(Gfx_log::MAIN_LOOP, "window render loop");
        // Swap buffers
        // Dark blue background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.3f, 1.0f);

        glm::mat4 cross_mvp(1.0);
        _pos_col->m4_mvp = cross_mvp;
        _pos_col->activate();
        _crosshair->draw();
        
        drawcall();
        glfwSwapBuffers(_window);
        glfwPollEvents();

        if (glfwWindowShouldClose(_window)) {
            return true;
        }
    }

    return false;
}
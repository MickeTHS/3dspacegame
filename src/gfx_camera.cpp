#include "engine/gfx_camera.h"
#include <glm/gtx/transform.hpp>
#include "pch.h"

/*


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 );
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;



void computeMatricesFromInputs(){

    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Reset mouse position for next frame
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Compute new orientation
    horizontalAngle += mouseSpeed * float(1024/2 - xpos );
    verticalAngle   += mouseSpeed * float( 768/2 - ypos );

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f/2.0f),
        0,
        cos(horizontalAngle - 3.14f/2.0f)
    );

    // Up vector
    glm::vec3 up = glm::cross( right, direction );

    // Move forward
    if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
        position += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
        position -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        position += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        position -= right * deltaTime * speed;
    }

    float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

    // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    ViewMatrix       = glm::lookAt(
                                position,           // Camera is here
                                position+direction, // and looks here : at the same position, plus "direction"
                                up                  // Head is up (set to 0,-1,0 to look upside-down)
                           );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}
*/

Gfx_camera::Gfx_camera() {
    
}

void Gfx_camera::init(float width, float height) {
    _proj = glm::perspective(glm::radians(90.0f), width / height, 0.1f, 100.0f);
    _pos = glm::vec3(0, 0, -10.0f);
    _lookat = glm::vec3(0, 0, 0);
}

void Gfx_camera::set_yaw(float y) {
    _yaw = y;
}

void Gfx_camera::set_pitch(float y) {
    _pitch = y;
}

glm::mat4 Gfx_camera::get_proj() {
    return _proj;
}

glm::mat4 Gfx_camera::get_view() {
    return _view;
}

glm::vec3 Gfx_camera::get_pos() {
    return _pos;
}

glm::vec3 Gfx_camera::get_lookat() {
    return glm::vec3(_view[2][0], _view[2][1], _view[2][2]);
}

void Gfx_camera::update() {
    //roll can be removed from here. because is not actually used in FPS camera
    glm::mat4 mat_roll = glm::mat4(1.0f); //identity matrix; 
    glm::mat4 mat_pitch = glm::mat4(1.0f); //identity matrix
    glm::mat4 mat_yaw = glm::mat4(1.0f); //identity matrix

    //roll, pitch and yaw are used to store our angles in our class
    mat_roll = glm::rotate(mat_roll, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    mat_pitch = glm::rotate(mat_pitch, _pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    mat_yaw = glm::rotate(mat_yaw, _yaw, glm::vec3(0.0f, 1.0f, 0.0f));

    //order matters
    glm::mat4 rotate = mat_roll * mat_pitch * mat_yaw;

    glm::mat4 translate = glm::mat4(1.0f);
    translate = glm::translate(translate, -_pos);

    _view = rotate * translate;

    printf("lookat: %f, %f, %f\n", _view[2][0], _view[2][1], _view[2][2]);
}
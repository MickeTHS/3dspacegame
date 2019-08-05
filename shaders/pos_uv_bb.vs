#version 430 core

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec2 v_uv;


// GLSL Hacker automatic uniforms:
uniform mat4 mvp;
uniform mat4 model_view;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

//uniform int spherical; // 1 for spherical; 0 for cylindrical

out vec2 out_uv;

void main()
{
    
    gl_Position = proj * model_view * vec4(v_pos, 1.0);;

  
  out_uv = v_uv;
}
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;

out vec3 out_fragpos;
out vec3 out_norm;
out vec2 out_uv;

uniform mat4 mvp;
uniform mat4 model;

void main(){
	
	gl_Position = mvp * vec4(pos, 1);
	
  out_fragpos = vec3(model * vec4(pos, 1));
	out_uv = uv;
  out_norm = norm;
}

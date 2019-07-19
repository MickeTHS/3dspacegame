#version 330 core

in vec3 out_color;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D tex0;
uniform vec4      highlight;

void main(){
	color = vec4(out_color, 1.0) * highlight;
}
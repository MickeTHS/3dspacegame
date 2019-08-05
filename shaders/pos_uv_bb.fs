#version 430 core
in vec2 out_uv;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D tex0;
uniform vec4      highlight;

void main(){
    //color = texture( tex0, out_uv ).rgba * highlight;;
    color = vec4(1.0, 1.0, 1.0, 1.0);
}
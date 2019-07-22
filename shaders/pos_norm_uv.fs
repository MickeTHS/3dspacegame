#version 430 core
in vec3 out_fragpos;
in vec3 out_norm;
in vec2 out_uv;


// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D tex0;
uniform vec4      highlight;
uniform vec3      light0pos;

void main() {
    vec3 lightDir = normalize(light0pos - out_fragpos);
    float diff = max(dot(out_norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.8, 0.4, 0.4);

    color = vec4(diffuse, 1.0) * texture( tex0, out_uv ).rgba * highlight;
}
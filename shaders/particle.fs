#version 430 core

in vec2 uv;
in vec4 color;

layout(binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 frag;

void main()
{
    vec4 c = texture(tex, uv);
    frag = c * color;

    frag = vec4(1.0, 1.0, 1.0, 1.0);
}

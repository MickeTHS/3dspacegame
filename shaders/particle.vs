#version 430 core

struct Particle
{
    vec3 pos;
    float rot;
    float size;
    float anim;
    uint color;
};

layout(std430, binding = 0) buffer P_data
{
    Particle p_data[];
};

uniform mat4 rotation;
uniform mat4 proj_view;
uniform vec2 anim_params;

uniform vec2 vc[4] = vec2[](vec2(0, 0), vec2(0, 1), vec2(1, 1), vec2(1, 0));

out gl_PerVertex
{
    vec4 gl_Position;
};

out vec2 uv;
out vec4 color;

vec2 rotate(vec2 p, float a)
{
    float s = sin(a);
    float c = cos(a);
    return vec2(p.x * c - p.y * s, p.x * s + p.y * c);
}

void main()
{
    vec4 pcolor = unpackUnorm4x8(p_data[gl_InstanceID].color);
    float anim = floor(p_data[gl_InstanceID].anim + 0.5) * anim_params.x;
    vec2 uv_anim;
    uv_anim.x = fract(anim);
    uv_anim.y = anim_params.y * (anim - uv_anim.x);

    uv = uv_anim + vc[gl_VertexID] * anim_params;

    vec2 pos = (2.0 * vc[gl_VertexID] - 1.0) * p_data[gl_InstanceID].size;
    pos = rotate(pos, p_data[gl_InstanceID].rot);
    gl_Position = rotation * vec4(pos.xy, 0.0, 1.0);
    gl_Position.xyz += p_data[gl_InstanceID].pos.xyz;
    gl_Position = proj_view * gl_Position;
    color = pcolor;
}

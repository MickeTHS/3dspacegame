#version 430 core

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_norm;
layout(location = 2) in vec2 v_uv;
layout(location = 3) in float f_bone;

out vec3 out_fragpos;
out vec3 out_norm;
out vec2 out_uv;
out float out_bone;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 model_view;
uniform mat4 pal[16];

void main(){
	//mat4 pm = pal[int(i_bone)];
    //vec4 lpos = pm * vec4(v_pos, 1.0);
    //gl_Position = mvp * lpos;

    mat4 pm = pal[int(f_bone)];
    vec4 lpos = pm * vec4(v_pos, 1.0);

    //wpos = (model * lpos).xyz;
    //vpos = (model_view * lpos).xyz;

    //gl_Position = mvp * vec4(v_pos, 1.0);
    gl_Position = mvp * lpos;

    /*mat4 pm = pal[int(v_index)];
    mat3 nm = mat3(model);
    norm = nm * mat3(pm) * v_norm;

    vec4 lpos = pm * vec4(v_pos, 1.0);

    wpos = (model * lpos).xyz;
    vpos = (model_view * lpos).xyz;

    gl_Position = mvp * lpos;

	gl_Position = mvp * vec4(pos, 1);*/
	
    out_fragpos = vec3(model * vec4(v_pos, 1.0));
	out_uv = v_uv;
    out_norm = v_norm;
    out_bone = f_bone;
}

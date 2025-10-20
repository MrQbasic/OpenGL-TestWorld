#version 430 core

layout(location = 0) in vec3 pos_position;
layout(location = 1) in vec2 a_texture;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_transform;

out vec2 tex;

void main()
{    
    //calc vertex pos
    vec4 pos_world = u_transform * vec4(pos_position, 1.0f);
    vec4 pos_toCam = u_view * pos_world;
    gl_Position = u_projection * pos_toCam;


    tex = a_texture;
}
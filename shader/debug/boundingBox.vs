#version 430 core

layout(location = 0) in vec3 pos_position;
layout(location = 1) in vec2 a_texture;
layout(location = 2) in vec3 normal_position;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform vec3 u_BoxPos;
uniform vec3 u_BoxSize;
uniform vec3 u_SunPos;

out vec3 pos;
out vec3 toSunVec;
out vec3 normal;

void main()
{    
    
    vec3 vertexPos = (pos_position + vec3(1)) / vec3(2);
    pos = vertexPos;
    vertexPos *= u_BoxSize;
    vertexPos += u_BoxPos;


    //calc vertex pos
    gl_Position = u_projection * u_view * vec4(vertexPos, 1.0f);

    toSunVec = u_SunPos - u_BoxPos;
    normal = normal_position;
}
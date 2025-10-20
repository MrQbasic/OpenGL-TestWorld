#version 430 core

layout(location = 0) in vec3 pos_position;
layout(location = 1) in vec2 a_texture;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform vec3 u_posShift;
uniform vec3 u_size;
uniform vec3 u_rot;

out vec2 posSurf;

void main()
{   
    vec3 rotation = u_rot * vec3(3.1415926535);

    float cx = cos(rotation.x), sx = sin(rotation.x);
    float cy = cos(rotation.y), sy = sin(rotation.y);
    float cz = cos(rotation.z), sz = sin(rotation.z);

    mat3 rotMat = mat3(
        cy * cz,                      cy * sz,                      -sy,
        sx * sy * cz - cx * sz,       sx * sy * sz + cx * cz,       sx * cy,
        cx * sy * cz + sx * sz,       cx * sy * sz - sx * cz,       cx * cy
    );

    gl_Position = u_projection * u_view * vec4(((pos_position * rotMat) * u_size + u_posShift), 1.0f);

    posSurf = a_texture * vec2(0.5) + vec2(0.15,0.25); // center it
}
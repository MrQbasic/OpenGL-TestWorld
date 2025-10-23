#version 430 core

layout(location = 0) in vec3 aPos;


#define structSize 11

layout(std430, binding = 0) buffer InstanceData {
    float data[];
};

uniform mat4 u_projection;
uniform mat4 u_view;
uniform vec3 u_pos;
uniform vec3 u_scale;

out vec3 tex;

float scaler = 0.08f;   //size of cubes


void main() {
    int id = gl_InstanceID;

    vec3 pos;
    pos.x = data[id*structSize+2];
    pos.y = data[id*structSize+3];
    pos.z = data[id*structSize+4];

    vec3 vel;
    vel.x = data[id*structSize+5];
    vel.y = data[id*structSize+6];
    vel.z = data[id*structSize+7];


    vec3 cageCenter = u_pos + (u_scale / 2);

    //velocity is the color
    tex = vel;

    vec3 vertexPos = (aPos * scaler) + (pos * u_scale) + cageCenter;

    gl_Position = u_projection * u_view * vec4(vertexPos, 1.0);
}




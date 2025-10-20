#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;

uniform mat4 u_projection;
uniform mat4 u_view;

uniform vec3 cameraPos;

void main()
{   
    vec3 posShifted = aPos * 200.0;

    posShifted.x -= cameraPos.x;
    posShifted.z -= cameraPos.z;

    FragPos = posShifted;
    
    gl_Position = u_projection * u_view * vec4(posShifted, 1.0f);
}
#version 330 core

in vec3 tex;
out vec4 color;

void main(){

    color = vec4(tex, 1.0f);
}
#version 430 core

in vec2 tex;

uniform sampler2D u_texture;

out vec4 color;

void main(){

    vec4 col_texture = texture(u_texture, tex);

    color = col_texture;
}
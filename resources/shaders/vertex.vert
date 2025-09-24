#version 330 core

layout (location=0) in vec3 vertex_position;
layout (location=1) in vec2 vertex_texture;
layout (location=2) in vec3 vertex_normal;

out vec3 fragment_color;

void main() {
    gl_Position = vec4(vertex_position, 1.0);
    fragment_color = vec3((vertex_position.z + 1.0) / 2.0);
}
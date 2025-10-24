#version 330 core

layout (location=0) in vec3 vertex_position;
layout (location=1) in vec2 vertex_texture;
layout (location=2) in vec3 vertex_normal;

uniform mat4 transformation;

out vec2 texture_coordinates;
out vec3 fragment_color;

void main() {
    gl_Position = transformation * vec4(vertex_position, 1.0);
    texture_coordinates = vec2(vertex_texture);
    fragment_color = vec3((vertex_position.z + 1.0) / 2.0);
}
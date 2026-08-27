#version 330 core

layout (location=0) in vec3 vertex_position;
layout (location=1) in vec2 vertex_texture;
layout (location=2) in vec3 vertex_normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 texture_coordinates;
out vec3 world_position;
out vec3 world_normal;
out vec3 world_camera_position;

void main() {
    gl_Position = projection * view * model * vec4(vertex_position, 1.0);
    
    texture_coordinates = vertex_texture;

    world_position = (model * vec4(vertex_position, 1.0)).xyz;
    world_normal = mat3(transpose(inverse(model))) * vertex_normal;
    world_camera_position = inverse(view)[3].xyz;
}
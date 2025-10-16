#version 330 core

in vec2 texture_coordinates;
in vec3 fragment_color;

uniform sampler2D my_texture;

out vec4 pixel_color;

void main() {
    // pixel_color = vec4(fragment_color, 1.0);
    // pixel_color = texture(my_texture, texture_coordinates);
    pixel_color = texture(my_texture, texture_coordinates) * vec4(fragment_color, 1.0);
}
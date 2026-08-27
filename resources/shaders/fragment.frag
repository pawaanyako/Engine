    #version 330 core

    in vec2 texture_coordinates;
    in vec3 world_position;
    in vec3 world_normal;
    in vec3 world_camera_position;

    uniform vec3 diffuse_color;
    uniform vec3 specular_color;
    uniform vec3 ambient_color;
    uniform sampler2D diffuse_texture;
    uniform sampler2D specular_texture;
    uniform sampler2D ambient_texture;

    out vec4 pixel_color;

    void main() {
        vec3 normalized_normal = normalize(world_normal);
        vec3 view_direction = normalize(world_camera_position - world_position);

        float light_factor = max(dot(normalized_normal, view_direction), 0.2);

        // pixel_color = vec4(diffuse_color, 1.0);
        // pixel_color = texture(diffuse_texture, texture_coordinates);

        vec4 deffuse_texture_color = texture(diffuse_texture, texture_coordinates);
        pixel_color = deffuse_texture_color * vec4(diffuse_color, 1.0) * light_factor;
    }
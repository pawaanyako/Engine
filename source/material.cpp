#include <material.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Material::Material(const std::string& name,
                   const std::vector<Texture>& textures,
                   float shininess,
                   float index_of_refraction,
                   float opacity,
                   int shading_mode) {
    Logger::debug("Material ctor(name, textures, shininess, index_of_refraction, opacity, shading_mode) name: ", name);
    name_ = name;
    textures_ = textures;
    shininess_ = shininess;
    index_of_refraction_ = index_of_refraction;
    opacity_ = opacity;
    shading_mode_ = shading_mode;
    bind_texture();
};

Material::~Material() {
    Logger::debug("Material dtor()");
};

const std::string Material::get_name() const {
    return name_;
}

const GLuint Material::get_id() const {
    return id_;
}

void Material::bind_texture() {
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    std::string map_Kd;
    glm::vec3 color{255};
    for (auto&& texture : textures_) {
        if (texture.type == Diffuse) {
            map_Kd = texture.texture_map;
            color = texture.color * 255.0f;
        }
    }

    if (map_Kd == "") {
        GLubyte pixel[] = {(GLubyte)color.r, (GLubyte)color.g, (GLubyte)color.b};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else {
        stbi_set_flip_vertically_on_load(true);
        int width, height, channels, req_channels = 3;
        stbi_uc* data = stbi_load(map_Kd.c_str(), &width, &height, &channels, req_channels);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        } else {
            glDeleteTextures(1, &id_);
            id_ = 0;
            Logger::error("Failed to load texture \"", map_Kd, "\" - ", stbi_failure_reason());
            return;
        }
        stbi_set_flip_vertically_on_load(false);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    Logger::debug("Material bind_texture - id: ", id_);
}

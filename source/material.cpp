#include <logger.h>
#include <material.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Material::Material() {
    Logger::debug("Material default ctr()");
};

Material::Material(const std::string& name, const std::string& map_Kd) {
    Logger::debug("Material ctr(name, map_Kd) name: " + name + " map_Kd: \"" + map_Kd + "\"");
    name_ = name;
    map_Kd_ = map_Kd;
    bind_texture();
};

Material::~Material() {
    Logger::debug("Material is deleted from scene");
};

std::string Material::get_name() const {
    return name_;
}

GLuint Material::get_id() const {
    return id_;
}

void Material::bind_texture() {
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    int width, height, channels, req_channels = 3;
    if (map_Kd_ == "none") {
        GLubyte white_pixel[] = {255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white_pixel);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else {
        stbi_uc* data = stbi_load(map_Kd_.c_str(), &width, &height, &channels, req_channels);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        } else {
            throw std::runtime_error("Failed to load texture \"" + map_Kd_ + "\" - " + std::string(stbi_failure_reason()));
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    Logger::debug("Material bind_texture - id: " + std::to_string(id_));
}

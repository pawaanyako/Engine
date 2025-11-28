#pragma once

#include <config.h>

enum TextureType { Ambient,
                   Diffuse,
                   Specular,
                   Roughness,
                   Metalic,
                   Sheen,
                   Emissive,
                   Normal,
                   None };

inline std::string texture_type_to_string(TextureType type) {
    switch (type) {
    case Ambient:
        return "Ambient";
    case Diffuse:
        return "Diffuse";
    case Specular:
        return "Specular";
    case Roughness:
        return "Roughness";
    case Metalic:
        return "Metalic";
    case Sheen:
        return "Sheen";
    case Emissive:
        return "Emissive";
    case Normal:
        return "Normal";
    case None:
        return "None";
    default:
        return "Unknown";
    }
}

struct Texture {
    TextureType type;
    glm::vec3 color;
    std::string texture_map;

    std::string to_string() const {
        return "type: " + texture_type_to_string(type) + ", color: " + glm::to_string(color) + ", texture_map: \"" + texture_map + "\"";
    }

    friend std::ostream& operator<<(std::ostream& os, const Texture& t) {
        os << "type: " << texture_type_to_string(t.type) << ", color: " << glm::to_string(t.color) << ", texture_map: " << t.texture_map;
        return os;
    }
};

class Material {
public:
    Material(const std::string& name,
             const std::vector<Texture>& textures,
             float shininess,
             float index_of_refraction,
             float opacity,
             int shading_mode);
    ~Material();

    const std::string get_name() const;
    const GLuint get_id() const;

private:
    std::string name_;
    GLuint id_;

    std::vector<Texture> textures_;
    float shininess_;
    float index_of_refraction_;
    float opacity_;
    int shading_mode_;

    void bind_texture();
};

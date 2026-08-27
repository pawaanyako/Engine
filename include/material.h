#pragma once

#include <config.h>

enum TextureType { None,
                   Diffuse,
                   Specular,
                   Ambient,
                   Emissive,
                   Height,
                   Normals,
                   Shininess,
                   Opacity,
                   Metallic_Roughness
};

inline std::string texture_type_to_string(TextureType type) {
    switch (type) {
    case None:
        return "None";
    case Diffuse:
        return "Diffuse";
    case Specular:
        return "Specular";
    case Ambient:
        return "Ambient";
    case Emissive:
        return "Emissive";
    case Height:
        return "Height";
    case Normals:
        return "Normals";
    case Shininess:
        return "Shininess";
    case Opacity:
        return "Opacity";
    case Metallic_Roughness:
        return "Metallic_Roughness";
    default:
        return "Unknown";
    }
}

struct Texture {
    GLuint id = 0;
    TextureType type = None;
    glm::vec3 color = glm::vec3(0.0f);
    std::string texture_map;

    std::string to_string() const {
        return "id: " + std::to_string(static_cast<unsigned int>(id)) + ", type: " + texture_type_to_string(type) + ", color: " + glm::to_string(color) + ", texture_map: \"" + texture_map + "\"";
    }

    friend std::ostream& operator<<(std::ostream& os, const Texture& t) {
        os << "id: " << std::to_string(static_cast<unsigned int>(t.id)) << ", type: " << texture_type_to_string(t.type) << ", color: " << glm::to_string(t.color) << ", texture_map: " << t.texture_map;
        return os;
    }
};

class Material {
public:
    Material(GLuint id,
             const std::string& name,
             const std::vector<Texture>& textures,
             float shininess,
             float index_of_refraction,
             float opacity,
             int shading_mode);
    ~Material();

    const GLuint get_id() const;
    const std::string& get_name() const;
    const std::vector<Texture>& get_textures() const;

private:
    GLuint id_;
    std::string name_;

    std::vector<Texture> textures_;
    float shininess_;
    float index_of_refraction_;
    float opacity_;
    int shading_mode_;

    void bind_textures();
};

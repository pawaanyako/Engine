#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <config.h>

class Material {
public:
    Material();
    Material(const std::string& name, const std::string& map_Kd);
    ~Material();

    std::string get_name() const;
    GLuint get_id() const;

private:
    std::string name_;
    GLuint id_;
    std::string type_;
    std::string map_Kd_;

    void bind_texture();
};

#endif

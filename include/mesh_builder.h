#ifndef MESH_BUILDER_H_
#define MESH_BUILDER_H_

#include <config.h>
#include <maths.h>
#include <mesh.h>

class MeshBuilder {
public:
    MeshBuilder();
    ~MeshBuilder();

    void add_triangle(vec3<GLfloat> v1, vec3<GLfloat> v2, vec3<GLfloat> v3);
    void add_triangle(vec3<GLfloat> v1, vec3<GLfloat> v2, vec3<GLfloat> v3, vec2<GLfloat> vt1, vec2<GLfloat> vt2, vec2<GLfloat> vt3);
    void add_quad(vec3<GLfloat> v1, vec3<GLfloat> v2, vec3<GLfloat> v3, vec3<GLfloat> v4);
    void add_quad(vec3<GLfloat> v1, vec3<GLfloat> v2, vec3<GLfloat> v3, vec3<GLfloat> v4, vec2<GLfloat> vt1, vec2<GLfloat> vt2, vec2<GLfloat> vt3, vec2<GLfloat> vt4);
    
    Mesh build();

private:
    std::vector<vec3<GLfloat>> vertex_positions_;
    std::vector<vec2<GLfloat>> vertex_textures_;
    std::vector<vec3<GLfloat>> vertex_normals_;
    std::vector<GLuint> vertex_indices_;
};
#endif

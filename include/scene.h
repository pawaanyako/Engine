#ifndef SCENE_H_
#define SCENE_H_

#include <config.h>
#include <mesh.h>

class Scene {
public:
    Scene();
    ~Scene();

    void load_mesh(Mesh mesh);
    void generate_vaos_vbos_ebos();
    void draw_meshes();

private:
    std::vector<Mesh> meshes_;
};

#endif

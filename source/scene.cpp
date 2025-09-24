#include <scene.h>

#include <thread>

Scene::Scene() {
}

Scene::~Scene() {
}

void Scene::load_mesh(Mesh mesh) {
    meshes_.push_back(mesh);
    meshes_.back().generate_vao_vbo_ebo();
}

void Scene::draw_meshes() {
    for (auto&& mesh : meshes_) {
        mesh.draw();
    }
}
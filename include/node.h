#pragma once

#include <config.h>
#include <material.h>
#include <mesh.h>

class Node {
public:
    Node(const std::string& name,
         const std::shared_ptr<Node>& parent,
         const std::vector<GLuint>& mesh_ids,
         const std::vector<GLuint>& material_ids);
    ~Node();

    const std::string& get_name() const;
    const std::weak_ptr<Node>& get_parent() const;
    const std::vector<std::shared_ptr<Node>>& get_children() const;
    const std::vector<GLuint>& get_mesh_ids() const;
    const std::vector<GLuint>& get_material_ids() const;

    void add_child(const std::shared_ptr<Node>& child);

private:
    std::string name_;
    std::weak_ptr<Node> parent_;
    std::vector<std::shared_ptr<Node>> children_;
    std::vector<GLuint> mesh_ids_;
    std::vector<GLuint> material_ids_;
};

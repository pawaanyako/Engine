#include <config.h>
#include <node.h>

Node::Node(const std::string& name,
           const std::shared_ptr<Node>& parent,
           const std::vector<GLuint>& mesh_ids,
           const std::vector<GLuint>& material_ids) {
    std::string parent_name;
    if (parent.get() != nullptr) {
        parent_name = parent.get()->get_name();
    };
    Logger::debug("Node ctor(name, parent, mesh_ids, material_ids) - name: ", name, " parent_name: ", parent_name,
                  " mesh_ids.size(): ", mesh_ids.size(), " material_ids_.size(): ", material_ids.size());
    name_ = name;
    parent_ = parent;
    mesh_ids_ = mesh_ids;
    material_ids_ = material_ids;
}

Node::~Node() {
    Logger::debug("Node dtor()");
}

const std::string& Node::get_name() const {
    return name_;
}

const std::weak_ptr<Node>& Node::get_parent() const {
    return parent_;
}

const std::vector<std::shared_ptr<Node>>& Node::get_children() const {
    return children_;
}

const std::vector<GLuint>& Node::get_mesh_ids() const {
    return mesh_ids_;
}

const std::vector<GLuint>& Node::get_material_ids() const {
    return material_ids_;
}

void Node::add_child(const std::shared_ptr<Node>& child) {
    children_.push_back(std::move(child));
}
#include "node.h"
#include <glm/gtc/matrix_transform.hpp>


int Node::nextId = 0;

Node::Node(std::string name) {
    Id = nextId++;
    Name = name;
    IsActive = true;
    Transform = NodeTransform();
}

Ref<Node> Node::AddChild(Ref<Node> child) {
    child->Parent = this;
    Childs.push_back(child);
    return child;
}

void Node::RemoveChild(Node child) {
    printf("not implemented");
}


glm::mat4x4 Node::GetModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, Transform.position);
		model = glm::rotate(model, glm::radians(Transform.rotation.z), glm::vec3(0, 0, 1));
		model = glm::rotate(model, glm::radians(Transform.rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(Transform.rotation.x), glm::vec3(1, 0, 0));
		model = glm::scale(model, Transform.scale);

    return model;
}

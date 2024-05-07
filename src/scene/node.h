#pragma once

#include "core/Ref.h"
#include <string>
#include <glm/glm.hpp>

struct NodeTransform{
  glm::vec3 position;
  glm::vec3 scale;
  glm::vec3 rotation;

  NodeTransform(glm::vec3 pos = glm::vec3(0), glm::vec3 scale = glm::vec3(1), glm::vec3 rotation = glm::vec3(0))
  {
    this->position = pos;
    this->scale = scale;
    this->rotation = rotation;
  };
};

class Node {

	public:
		int Id;
		std::string Name;
		NodeTransform Transform;
		Node* Parent;
		std::vector<Ref<Node>> Childs;

		bool IsActive = true;


		Ref<Node> AddChild(Ref<Node> child);
		void RemoveChild(Node child);

		Node() : Id(nextId++), Transform(NodeTransform()) {};
		Node(std::string name);

		glm::mat4x4 GetModelMatrix() const;

		virtual ~Node() = default;

		private:
		int childIndex = 0;
		static int nextId;
};

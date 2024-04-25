#pragma once

#include "components/Transform.h"
#include "core/Ref.h"
#include <string>
#include <vector>

class Node {

	public:
		int Id;
		std::string Name;
		TransformC Transform;
		Node* Parent;
		std::vector<Ref<Node>> Childs;

		bool IsActive = true;


		Ref<Node> AddChild(Ref<Node> child);
		void RemoveChild(Node child);

		Node() : Id(nextId++), Transform(TransformC()) {};
		Node(std::string name);

		glm::mat4x4 GetModelMatrix() const;

		virtual ~Node() = default;

		private:
		int childIndex = 0;
		static int nextId;
};

#pragma once
#include "editor_window.h"
#include "scene/node.h"
#include <functional>

class SceneHierarchy : public EditorWindow {
  public:
    void SetSelectCallback(std::function<void(Node*)> callback = nullptr);
    void SelectNode(Node* node);
		void RenderNode(Ref<Node> node);
    bool IsFocused() override;
    void RenderWindow() override;
		void SetRoot(Ref<Node> rootNode);
	private:
		Ref<Node> selectedRoot;
};



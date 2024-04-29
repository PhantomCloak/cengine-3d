#pragma once
#include "editor_window.h"
#include "scene/node.h"
#include <map>

class Entity;
class EntityInspector : public EditorWindow {
  public:
    static void SetEntity(Node* entity);
    bool IsFocused() override;
    void RenderWindow() override;
    static Ref<Node> selectedNode;
		std::map<std::string, bool> ppfxSettings;
		float exposure = 0.5f;
  private:
};

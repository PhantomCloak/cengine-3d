#pragma once
#include "editor_window.h"
#include "scene/node.h"

class Entity;
class EntityInspector : public EditorWindow {
  public:
    static void SetEntity(Node* entity);
    bool IsFocused() override;
    void RenderWindow() override;
    static Ref<Node> selectedNode;
  private:
};

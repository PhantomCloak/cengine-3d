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
		float maxBias = 0.050;
		float minBias = 0.005;


		//float maxBias = 0.265;
		//float minBias = 0.079;
  private:
};

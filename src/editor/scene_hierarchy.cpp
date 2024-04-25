#include "scene_hierarchy.h"
#include "imgui.h"
#include "editor/entity_inspector.h"
#include "core/Ref.h"

std::function<void(Node*)> selectCallback;
int selectedNodeId = -1;

bool SceneHierarchy::IsFocused() {
    return ImGui::IsWindowFocused();
}

void SceneHierarchy::SetSelectCallback(std::function<void(Node*)> callback) {
    selectCallback = callback;
}

void SceneHierarchy::SelectNode(Node* node) {
	selectedNodeId = node->Id;
}

void SceneHierarchy::RenderNode(Ref<Node> node) {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

    if (node->Childs.size() == 0) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

		if (selectedNodeId == node->Id) {
            flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (ImGui::TreeNodeEx(node->Name.c_str(), flags)) {
        if (node->Childs.size() > 0)
            ImGui::Indent(0.5f);

        if (ImGui::IsItemClicked()) {
					selectedNodeId = node->Id;
					EntityInspector::selectedNode = node;
        }

        for (auto child : node->Childs) {
            RenderNode(child);
        }
        ImGui::TreePop();
    }
}

void SceneHierarchy::RenderWindow() {

    ImGui::Begin("Scene List");

		if(selectedRoot != nullptr)
		{
			RenderNode(selectedRoot);
		}

    ImGui::End();
}


void SceneHierarchy::SetRoot(Ref<Node> rootNode) {
	selectedRoot = rootNode;
}

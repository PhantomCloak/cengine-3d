#include "entity_inspector.h"
#include "imgui.h"

Ref<Node> EntityInspector::selectedNode = nullptr;

bool EntityInspector::IsFocused() {
    return ImGui::IsWindowFocused();
}
void EntityInspector::SetEntity(Node* entity) {
    // EntityInspector::selectedNode = entity;
}

void EntityInspector::RenderWindow() {
    if (ImGui::Begin("Properties")) {
        if (selectedNode != nullptr && EntityInspector::selectedNode->IsActive) {
            ImGui::Text("ID: %d", selectedNode->Id);

            ImGui::BeginGroupPanel("Transform");

            ImGui::InputFloat(_labelPrefix("pos x:").c_str(), &selectedNode->Transform.pos.x, 1);
            ImGui::InputFloat(_labelPrefix("pos y:").c_str(), &selectedNode->Transform.pos.y, 1);
            ImGui::InputFloat(_labelPrefix("pos z:").c_str(), &selectedNode->Transform.pos.z, 1);

            ImGui::Spacing();
            ImGui::Spacing();

						ImGui::InputFloat(_labelPrefix("scale x:").c_str(), &selectedNode->Transform.scale.x, 1);
						ImGui::InputFloat(_labelPrefix("scale y:").c_str(), &selectedNode->Transform.scale.y, 1);
						ImGui::InputFloat(_labelPrefix("scale z:").c_str(), &selectedNode->Transform.scale.z, 1);
 
            ImGui::Spacing();
            ImGui::Spacing();

						ImGui::InputFloat(_labelPrefix("rot x:").c_str(), &selectedNode->Transform.rotation.x, 1);
						ImGui::InputFloat(_labelPrefix("rot y:").c_str(), &selectedNode->Transform.rotation.y, 1);
						ImGui::InputFloat(_labelPrefix("rot z:").c_str(), &selectedNode->Transform.rotation.z, 1);

            ImGui::EndGroupPanel();
        }
        ImGui::End();
    }
}

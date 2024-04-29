#include "entity_inspector.h"
#include "imgui.h"
#include "render/backends/opengl/DirectonalLight.h"

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

            ImGui::InputFloat(_labelPrefix("pos x:").c_str(), &selectedNode->Transform.position.x, 1);
            ImGui::InputFloat(_labelPrefix("pos y:").c_str(), &selectedNode->Transform.position.y, 1);
            ImGui::InputFloat(_labelPrefix("pos z:").c_str(), &selectedNode->Transform.position.z, 1);

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

						if(selectedNode->Name == "Light")
						{
							ImGui::BeginGroupPanel("Light");

							auto ptrLight = std::dynamic_pointer_cast<DirectionalLight>(selectedNode);

							if(ImGui::InputFloat(_labelPrefix("Ambient").c_str(), &ptrLight->Ambient.x, 0.1f)) {
								ptrLight->Ambient.y = ptrLight->Ambient.x;
								ptrLight->Ambient.z = ptrLight->Ambient.x;
							}

            	ImGui::Spacing();
            	ImGui::Spacing();

							if(ImGui::InputFloat(_labelPrefix("Diffuse x: ").c_str(), &ptrLight->Diffuse.x, 0.1f)) {
								ptrLight->Diffuse.y = ptrLight->Diffuse.x;
								ptrLight->Diffuse.z = ptrLight->Diffuse.x;
							}

            	ImGui::Spacing();
            	ImGui::Spacing();

							if(ImGui::InputFloat(_labelPrefix("Specular: ").c_str(), &ptrLight->Specular.x, 0.1f)) {
								ptrLight->Specular.y = ptrLight->Specular.x;
								ptrLight->Specular.z = ptrLight->Specular.x;
							}

							ImGui::EndGroupPanel();
						}

				}
				ImGui::End();

				if (ImGui::Begin("POST FX Settings")) {
						ImGui::Checkbox("Gamma Correction: ", &ppfxSettings["gamma_correction"]);
						ImGui::Checkbox("Use Reinhard: ", &ppfxSettings["use_reinhard"]);
						ImGui::InputFloat("Exposure: ", &exposure, 0.1);
				}
				ImGui::End();
		}
}

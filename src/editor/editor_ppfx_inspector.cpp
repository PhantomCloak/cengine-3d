#include "editor_ppfx_inspector.h"
#include "imgui.h"

EditorPPFXInspector::EditorPPFXInspector(std::shared_ptr<Editor> editor) {
  editorInstance = editor;
	brightColorCuttoff.x = 0.2126;
	brightColorCuttoff.y = 0.7152;
	brightColorCuttoff.z = 0.2126;
}

bool EditorPPFXInspector::IsFocused() {
    return ImGui::IsWindowFocused();
}

void EditorPPFXInspector::RenderWindow() {

	const char* CorrectionTypeNames[] = { "ACES", "Reinhard", "Flat" };
	static int currentItem = 0;


  if (ImGui::Begin("POST FX Settings")) {

		if(ImGui::Combo(_labelPrefix("CC Type: ").c_str(), &currentItem, CorrectionTypeNames, sizeof(CorrectionTypeNames))) {

		}

		ImGui::Spacing();

		ImGui::InputFloat(_labelPrefix("B. Cutoff x: ").c_str(), &brightColorCuttoff.x, 0.05f);
		ImGui::InputFloat(_labelPrefix("B. Cutoff y: ").c_str(), &brightColorCuttoff.y, 0.05f);
		ImGui::InputFloat(_labelPrefix("B. Cutoff z: ").c_str(), &brightColorCuttoff.z, 0.05f);

		ImGui::Spacing();

    ImGui::InputFloat("Exposure: ", &exposure, 0.1);
  }
  ImGui::End();
}

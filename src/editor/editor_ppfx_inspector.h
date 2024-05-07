#pragma once
#include "editor/editor_window.h"
#include "glm/glm.hpp"

enum CorrectionType : int { 
	ACES = 0,
	Reinhard = 1,
	Flat = 2
};

class Editor;

class EditorPPFXInspector : public EditorWindow {
  public:
    EditorPPFXInspector(std::shared_ptr<Editor> editor);
    void RenderWindow() override;
    bool IsFocused() override;
		float exposure = 0.5f;

		glm::vec3 brightColorCuttoff;
		CorrectionType currentType;
		
  private:
    std::shared_ptr<Editor> editorInstance;
};

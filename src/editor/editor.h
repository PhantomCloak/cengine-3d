#pragma once

#include "../render/render_primitives.h"
#include "../render/render.h"
#include "editor/editor_menu_bar.h"
#include "editor/fileview.h"
#include "editor/log_viewer.h"
#include "editor/system_explorer.h"
#include "editor/entity_inspector.h"
#include "editor/viewport.h"
#include "editor/scene_hierarchy.h"

class Editor {
    public:
    Editor() = default;
    void Init(CommancheRenderer* renderer);
    static glm::vec2 GetCursorPosition();
    static glm::vec2 GetViewPortSize();
    void Render();
    std::shared_ptr<EditorViewPort> viewport;
    std::shared_ptr<SceneHierarchy> sceneList;
    static std::shared_ptr<Editor> Instance;
    std::shared_ptr<EntityInspector> entityInspector;
    private:
    void Keybindings();
    FileView* fileView;
    SystemExplorer* explorer;
    std::shared_ptr<LogView> logView;
    std::unique_ptr<CommancheRenderer> renderer;
    std::shared_ptr<EditorMenuBar> menuBar;
};

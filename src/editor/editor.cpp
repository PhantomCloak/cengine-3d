#include "editor.h"
#include "../assetmgr/AssetManager.h"
#include "../io/keyboard.h"
#include "../libs/imgui/imgui.h"
#include "editor_style.h"
#include "imgui.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

std::shared_ptr<Editor> Editor::Instance;

void Editor::Init(CommancheRenderer* renderer) {
    Log::Inf("Editor booting...");

    Instance = std::shared_ptr<Editor>(this);

    entityInspector = std::make_shared<EntityInspector>();
    logView = std::make_shared<LogView>();
    //importer = std::make_shared<TileMapImporter>();
    tilePlacer = std::make_shared<TilePlacer>();
    viewport = std::make_shared<EditorViewPort>();
    sceneList = std::make_shared<SceneHierarchy>();
    menuBar = std::make_shared<EditorMenuBar>(Instance);

    sceneList->SetSelectCallback([this](Node* entity) {
        entityInspector->SetEntity(entity);
    });

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

#if RENDER_BACKEND_OPENGL
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)renderer->wnd, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
#endif

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;


    EditorStyle::Init();

    AssetManager::AddTexture("axis", "./assets/editor/axis.png");
    AssetManager::AddTexture("file", "./assets/editor/file-icon.png");
    AssetManager::AddTexture("folder", "./assets/editor/folder-icon.png");
    AssetManager::AddTexture("gear", "./assets/editor/gear.png");

    Log::Inf("Editor started");

    //EditorSystems::Init(Scene::ecs, this);
}

void Editor::Keybindings() {
    int offsetAmount = 5;
    if (Keyboard::IsKeyPressing(KeyCode::Key_RArrow)) {
        CommancheRenderer::Instance->OffsetCamera(0, offsetAmount);
    }

    if (Keyboard::IsKeyPressing(KeyCode::Key_LArrow)) {
        CommancheRenderer::Instance->OffsetCamera(0, -offsetAmount);
    }

    if (Keyboard::IsKeyPressing(KeyCode::Key_UArrow)) {
        CommancheRenderer::Instance->OffsetCamera(-offsetAmount, 0);
    }
    if (Keyboard::IsKeyPressing(KeyCode::Key_DArrow)) {
        CommancheRenderer::Instance->OffsetCamera(offsetAmount, 0);
    }
}


void renderDockingSpace() {
    static bool opt_fullscreen = true;
    static ImGuiWindowFlags window_flags = 0;
    static bool p_open = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;


    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &p_open, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    ImGui::End();
}

glm::vec2 Editor::GetCursorPosition() {
    return Instance->viewport->ViewportCursorPos;
}

glm::vec2 Editor::GetViewPortSize() {
    return Instance->viewport->ViewportSize;
}

void Editor::Render() {
    Keybindings();

#if RENDER_BACKEND_OPENGL
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
#endif
    menuBar->RenderWindow();
    renderDockingSpace();


    if (menuBar->enabledWindows[EDITOR_SHOW_MAP_EDITOR]) {
        tilePlacer->RenderWindow();
    }

    sceneList->RenderWindow();
    entityInspector->RenderWindow();
    logView->RenderWindow();

    viewport->RenderWindow();

#if RENDER_BACKEND_OPENGL
    ImGui::Render();

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#endif
}

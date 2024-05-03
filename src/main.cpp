#include "editor/editor.h"
#include "game/game.h"
#include "render/render.h"

int main(int argc, char* argv[]) {
    Game game;
    game.Initialize();
#if EDITOR
		Editor* editor = new Editor();
		editor->Init(game.renderer.get());
		editor->sceneList->SetRoot(game.Root);
#endif
		// TODO brak dep
    game.Setup();

    while (game.isRunning) {
        game.ProcessInput();
        CommancheRenderer::Instance->RenderStart();
        game.Update();
        game.Render();

#if EDITOR
				editor->Render();
#endif
        CommancheRenderer::Instance->RenderEnd();
    }
    return 0;
}

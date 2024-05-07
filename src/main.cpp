#include "editor/editor.h"
#include "game/game.h"
#include "render/render.h"

int main(int argc, char* argv[]) {
    Game game;
#if EDITOR
		Editor* editor = new Editor();
		editor->Init();
		editor->sceneList->SetRoot(game.Root);
#endif
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

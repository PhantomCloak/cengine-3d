#pragma once
#include "render/render.h"
#include "scene/node.h"

const int FPS = 60;
const int FRAME_TIME_LENGTH = (1000 / FPS);

class Game {
    public:
    Game();
    ~Game();
    void Setup();
    void Run();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();

    bool isRunning;

		Ref<Node> Root;
    Ref<CommancheRenderer> renderer;
    private:
    double tickLastFrame = 0;
};

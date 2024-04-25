#include "game.h"
#include "../assetmgr/AssetManager.h"
#include "../common/common.h"
#include "../io/cursor.h"
#include "../io/filesystem.h"
#include "../io/keyboard.h"
#include "log/log.h"
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "editor/editor.h"
#include "render/Camera.h"
#include "render/backends/opengl/Model.h"
#include "render/backends/opengl/Shader.h"

Shader* lightingShader;
Shader* lightCubeShader;
Ref<Model> sampleModel;
Ref<Model> sampleModel2;
Camera* cam;

unsigned int fbo = -1;
unsigned int ppfxTexture = -1;
unsigned int imguiTexture = -1;
unsigned int depthBuffer;

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
};

void CreateRenderBuffer(int width, int height) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &ppfxTexture);

    glBindTexture(GL_TEXTURE_2D, ppfxTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Game::Game() {
    isRunning = false;
    Log::Inf("Game Constructor Called");
}

Game::~Game() {
    Log::Inf("Game Deconstructor Called");
}


void Game::Initialize() {
    isRunning = true;

    renderer = CreateRef<CommancheRenderer>();
    Root = CreateRef<Node>("Root");

    AssetManager::Initialize(renderer.get());

    renderer->Initialize("Twelve Villages", 1920, 1080);

    Log::Inf("Render backend has created.");

    renderer->InitializeShaders("./src/shaders");

    Log::Inf("Shaders are loaded.");

    Keyboard::Setup();
    Cursor::Setup(renderer->wnd);

    Log::Inf("Inputs are initialized.");
}

int selectedTextureId;
unsigned int lightCubeVAO;
void Game::Setup() {
    std::vector<std::string> files = FileSys::GetFilesInDirectory("./assets/tile_maps");

    for (auto file : files) {
        if (FileSys::GetFileExtension(file) != "png")
            continue;

        AssetManager::AddTexture(FileSys::GetFileName(file), file);
    }

    AssetManager::AddTexture("desert", "assets/images/desert.png");

    Log::Warn("Engine is starting");

    cam = new Camera(glm::vec3(0.0f, 10.0f, 3.0f));
    lightingShader = new Shader("default.vs", "default.fs");
    lightCubeShader = new Shader("light_cube.vs", "light_cube.fs");

     sampleModel = CreateRef<Model>("sponza.obj");
    //sampleModel2 = CreateRef<Model>("tile.obj");

    Root->AddChild(sampleModel);
    //Root->AddChild(sampleModel2);

		//Root->Childs[0]->Transform.pos = glm::vec3(0, 9, 0);
    //Root->Childs[0]->Transform.scale = glm::vec3(10.0f, 1.0f, 10.0f);

    CommancheRenderer::Instance->AddOnViewportChangeEvent([](int width, int height) {
        // CreateRenderBuffer(width / 2, height / 2);
    });


    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Game::Update() {
    int timeToWait = FRAME_TIME_LENGTH - (getTime() - tickLastFrame);
    static int fps = 0;
    static int fpsLastCheck = 0;
    fps++;
    if (timeToWait > 0 && timeToWait <= FRAME_TIME_LENGTH) {
        sleepProgram(timeToWait);
    }

    if ((getTime() - fpsLastCheck) > 1000) {
        fps = 0;
        fpsLastCheck = getTime();
    }

    tickLastFrame = getTime();

    Keyboard::FlushPressedKeys();
}
glm::vec2 lastCpos = glm::vec2(0);

void Game::ProcessInput() {
    glm::vec2 cpos = Cursor::GetCursorPosition();
    glm::vec2 cposOffset = glm::vec2(0);

    cposOffset.x = cpos.x - lastCpos.x;
    cposOffset.y = cpos.y - lastCpos.y;

    if (Keyboard::IsKeyPressing(Key_W)) {
        cam->ProcessKeyboard(FORWARD, 1.1f);
    } else if (Keyboard::IsKeyPressing(Key_S)) {
        cam->ProcessKeyboard(BACKWARD, 1.1f);
    } else if (Keyboard::IsKeyPressing(Key_A)) {
        cam->ProcessKeyboard(LEFT, 1.1f);
    } else if (Keyboard::IsKeyPressing(Key_D)) {
        cam->ProcessKeyboard(RIGHT, 1.1f);
    } else if (Keyboard::IsKeyPressing(Key_SPACE)) {
        cam->ProcessKeyboard(UP, 1.1f);
    } else if (Keyboard::IsKeyPressing(Key_C)) {
        cam->ProcessKeyboard(DOWN, 1.1f);
    } else if (Keyboard::IsKeyPressing(Key_Q)) {
			cam->Yaw += 1.0f;
			cam->updateCameraVectors();
    } else if (Keyboard::IsKeyPressing(Key_E)) {
			cam->Yaw -= 1.0f;
			cam->updateCameraVectors();
    }

    // if(Editor::Instance->viewport->IsFocused())
    //{
    // cam->ProcessMouseMovement(cposOffset.x, cposOffset.y);
    // }


    lastCpos = cpos;
    Keyboard::Poll();
}

glm::vec3 lightPos(-3.2f, 750.0f, 2.0f);
//glm::vec3 lightPos(0.0f, 10.0f, 0.0f);
glm::vec3 cubePos(0.0f, 0.0f, 0.0f);

void Game::Render() {
    int renderW = CommancheRenderer::screenWidth;
    int renderH = CommancheRenderer::screenHeight;

    if (fbo == -1) {
        CreateRenderBuffer(renderW, renderH);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ppfxTexture, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, renderW, renderH);
    glClearColor(0.0f, 0.13f, 0.0, 1.0f);
    glEnable(GL_DEPTH_TEST);


    lightingShader->use();
    lightingShader->setVec3("lightPos", lightPos);
    lightingShader->setVec3("light.position", lightPos);
    lightingShader->setVec3("viewPos", cam->Position);

    lightingShader->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    lightingShader->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    lightingShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    lightingShader->setFloat("material.shininess", 64.0f);

    glm::mat4 projection = glm::perspective(glm::radians(cam->Zoom), (float)renderW / (float)renderH, 0.1f, 10000.0f);

    glm::mat4 view = cam->GetViewMatrix();
    lightingShader->setMat4("projection", projection);
    lightingShader->setMat4("view", view);

    // world transformation
    glm::mat4 mainCubeTransform = glm::mat4(1.0f);
    mainCubeTransform = glm::translate(mainCubeTransform, cubePos);
    mainCubeTransform = glm::scale(mainCubeTransform, glm::vec3(1));
    lightingShader->setMat4("model", mainCubeTransform);

    sampleModel->Draw(*lightingShader);
    //sampleModel2->Draw(*lightingShader);

    glm::mat4 cameraTransform = glm::mat4(1.0f);
    cameraTransform = glm::translate(cameraTransform, lightPos);
		cameraTransform = glm::scale(cameraTransform, glm::vec3(10));
    //cameraTransform = glm::scale(cameraTransform, glm::vec3(0.1f));

    lightCubeShader->use();
    lightCubeShader->setMat4("projection", projection);
    lightCubeShader->setMat4("view", view);
    lightCubeShader->setMat4("model", cameraTransform);

    glBindVertexArray(lightCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    renderer->textureColorbuffer = ppfxTexture;
}


void Game::Destroy() {
    isRunning = false;
}

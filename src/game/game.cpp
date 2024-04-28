#include "game.h"
#include "../assetmgr/AssetManager.h"
#include "../common/common.h"
#include "../io/cursor.h"
#include "../io/keyboard.h"
#include "log/log.h"
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "render/Camera.h"
#include "render/backends/opengl/DirectonalLight.h"
#include "render/backends/opengl/Model.h"
#include "render/backends/opengl/Shader.h"
#include "constants.h"

#if EDITOR
#include "editor/editor.h"
#endif


Shader *defaultShader;
Shader *drawCubeShader;
Shader *skyboxShader;
Ref<Model> sampleModel;
Ref<Model> lightModel;
Camera *camera;

Game::Game() {
  isRunning = false;
  Log::Inf("Game Constructor Called");
}

Game::~Game() { Log::Inf("Game Deconstructor Called"); }

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

Ref<DirectionalLight> light;

void Game::Setup() {
  Log::Warn("Engine is starting");

  camera = new Camera(glm::vec3(0.0f, 2.0f, 3.0f));
	static glm::vec3 lightPos(0.0f, 500.0f, 0.0f);

  light = CreateRef<DirectionalLight>(glm::vec3(0.2f), glm::vec3(0.5f),
                                      glm::vec3(1));
	light->Transform.pos = lightPos;
	light->Transform.scale = glm::vec3(10);

  defaultShader = new Shader("assets/shaders/default.vs", "assets/shaders/default.fs");
  skyboxShader = new Shader("assets/shaders/skybox.vs", "assets/shaders/skybox.fs");

  sampleModel = CreateRef<Model>("assets/models/sponza.obj");
  lightModel = CreateRef<Model>("assets/models/cube.obj");

  Root->AddChild(sampleModel);
  Root->AddChild(light);
  Root->AddChild(lightModel);
}

void Game::Update() {
  static int fpsLastCheck = 0;

  int timeToWait = FRAME_TIME_LENGTH - (getTime() - tickLastFrame);
  if (timeToWait > 0 && timeToWait <= FRAME_TIME_LENGTH) {
    sleepProgram(timeToWait);
  }

  if ((getTime() - fpsLastCheck) > 1000) {
    fpsLastCheck = getTime();
  }

  tickLastFrame = getTime();

  Keyboard::FlushPressedKeys();
}


void Game::ProcessInput() {
	static glm::vec2 lastCpos = glm::vec2(0);
  glm::vec2 cpos = Cursor::GetCursorPosition();
  glm::vec2 cposOffset = glm::vec2(0);

  cposOffset.x = cpos.x - lastCpos.x;
  cposOffset.y = cpos.y - lastCpos.y;
  const float mulSpeed = 5.0f;

  if (Keyboard::IsKeyPressing(Key_W)) {
    camera->ProcessKeyboard(FORWARD, 1.1f * mulSpeed);
  } else if (Keyboard::IsKeyPressing(Key_S)) {
    camera->ProcessKeyboard(BACKWARD, 1.1f * mulSpeed);
  } else if (Keyboard::IsKeyPressing(Key_A)) {
    camera->ProcessKeyboard(LEFT, 1.1f * mulSpeed);
  } else if (Keyboard::IsKeyPressing(Key_D)) {
    camera->ProcessKeyboard(RIGHT, 1.1f * mulSpeed);
  } else if (Keyboard::IsKeyPressing(Key_SPACE)) {
    camera->ProcessKeyboard(UP, 1.1f * mulSpeed);
  } else if (Keyboard::IsKeyPressing(Key_C)) {
    camera->ProcessKeyboard(DOWN, 1.1f * mulSpeed);
  } else if (Keyboard::IsKeyPressing(Key_Q)) {
    camera->Yaw += 1.0f;
    camera->updateCameraVectors();
  } else if (Keyboard::IsKeyPressing(Key_E)) {
    camera->Yaw -= 1.0f;
    camera->updateCameraVectors();
  }

	if(Editor::Instance->viewport->IsFocused())
		camera->ProcessMouseMovement(cposOffset.x, cposOffset.y);

  lastCpos = cpos;
  Keyboard::Poll();
}


void RenderScene(Shader* shader, glm::mat4 projectionMat, glm::mat4 viewMat) {
  defaultShader->use();
  defaultShader->setVec3("viewPos", camera->Position);

  light->Draw(*defaultShader);

  defaultShader->setFloat("material.shininess", 64.0f);

  defaultShader->setMat4("projection", projectionMat);
  defaultShader->setMat4("view", viewMat);

  sampleModel->Draw(*defaultShader);

	lightModel->Transform = light->Transform;
	lightModel->Draw(*defaultShader);
}

void Game::Render() {
	static glm::vec3 cubePos(0.0f, 0.0f, 0.0f);

  int renderWitdh = CommancheRenderer::screenWidth;
  int renderHeight = CommancheRenderer::screenHeight;

	// Viewport
	static unsigned int viewportFbo = -1;
	static unsigned int viewporFboDepthBuffer = -1;
	static unsigned int viewportFboOutTex = -1, viewportDepthFboOutTex = -1;

	static unsigned int depthBufferFbo = -1, depthBufferTex = -1;


	// Skybox
	static unsigned int skyboxVAO = -1, skyboxVBO = -1, skyboxCubeTextureId = -1;

	if(skyboxVBO == -1 && skyboxVBO == -1 && skyboxCubeTextureId == -1)
	{
  	glGenVertexArrays(1, &skyboxVAO);
  	glGenBuffers(1, &skyboxVBO);
  	glBindVertexArray(skyboxVAO);
  	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  	glEnableVertexAttribArray(0);
  	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

		std::vector<std::string> faces{"skybox/right.jpg", "skybox/left.jpg",
		                              "skybox/top.jpg",   "skybox/bottom.jpg",
		                              "skybox/front.jpg", "skybox/back.jpg"};
		skyboxCubeTextureId = AssetManager::LoadCubeMap(faces);
	}

  if (viewportFbo == -1) {
    glGenFramebuffers(1, &viewportFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, viewportFbo);

    glGenTextures(1, &viewportFboOutTex);
    glGenTextures(1, &viewportDepthFboOutTex);

    glBindTexture(GL_TEXTURE_2D, viewportFboOutTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWitdh, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glBindTexture(GL_TEXTURE_2D, viewportDepthFboOutTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWitdh, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &viewporFboDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, viewporFboDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, renderWitdh, renderHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, viewporFboDepthBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, viewportFbo);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, viewportFboOutTex, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, viewportDepthFboOutTex, 0);
	GLenum drawBuffers2[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, drawBuffers2);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, renderWitdh, renderHeight);
  glClearColor(0.0f, 0.13f, 0.0, 1.0f);
  glEnable(GL_DEPTH_TEST);

  glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)renderWitdh / (float)renderHeight, 0.1f, 10000.0f);
	glm::mat4 view = camera->GetViewMatrix();

	RenderScene(defaultShader, projection, view);

  glDepthFunc(GL_LEQUAL);

  skyboxShader->use();
  view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
  skyboxShader->setMat4("view", view);
  skyboxShader->setMat4("projection", projection);
  // skybox cube
  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeTextureId);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Render Scene From Light

  renderer->textureColorbuffer = viewportFboOutTex;
  renderer->depthBuffer = viewportDepthFboOutTex;
}

void Game::Destroy() { isRunning = false; }

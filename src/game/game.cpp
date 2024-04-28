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
Shader *unlitShader;
Shader *shadowShader;
Shader *debugShader;

Ref<Model> sampleModel;
Ref<Model> lightModel;
Camera *camera;

Game::Game() {
  isRunning = false;
  Log::Inf("Game Constructor Called");
}
float yaw = 0, pitch = 0;

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

  light = CreateRef<DirectionalLight>(glm::vec3(0.2f), glm::vec3(0.5f),
                                      glm::vec3(1));
	light->Transform.position = glm::vec3(0, 2000, 0);
	light->Transform.rotation = glm::vec3(-70, 0, 0);

	light->Transform.scale = glm::vec3(10);

  defaultShader = new Shader("assets/shaders/default.vs", "assets/shaders/default.fs");
  skyboxShader = new Shader("assets/shaders/skybox.vs", "assets/shaders/skybox.fs");
  unlitShader = new Shader("assets/shaders/unlit.vs", "assets/shaders/unlit.fs");
  shadowShader = new Shader("assets/shaders/shadow.vs", "assets/shaders/shadow.fs");
  debugShader = new Shader("assets/shaders/debug.vs", "assets/shaders/debug.fs");

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

//g
//GGkkkkkkklm::vec2 eulerToYawPitch(const glm::vec3& euler) {
//    float yaw = glm::degrees(glm::yaw(euler));
//    float pitch = glm::degrees(glm::pitch(euler));
//    return glm::vec2(yaw, pitch);
//}

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
		yaw += 1.0f;
    //camera->Yaw += 1.0f;
    camera->updateCameraVectors();
  } else if (Keyboard::IsKeyPressing(Key_E)) {
    //camera->Yaw -= 1.0f;
		yaw -= 1.0f;
    camera->updateCameraVectors();
  } else if(Keyboard::IsKeyPressing(Key_H))
	{
		pitch += 1.0f;

	} else if(Keyboard::IsKeyPressing(Key_J))
	{
		pitch -= 1.0f;

	}


	if(Editor::Instance->viewport->IsFocused())
		camera->ProcessMouseMovement(cposOffset.x, cposOffset.y);

  lastCpos = cpos;
  Keyboard::Poll();
}


void RenderScene(Shader* shader, glm::mat4 projectionMat, glm::mat4 viewMat) {
  shader->setVec3("viewPos", camera->Position);

  light->Draw(*shader);

  shader->setFloat("material.shininess", 64.0f);

  shader->setMat4("projection", projectionMat);
  shader->setMat4("view", viewMat);

  sampleModel->Draw(*shader);

	lightModel->Transform = light->Transform;
	lightModel->Draw(*shader);
}

int DebugRenderOffScreenImage(int renderWitdh, int renderHeight, int textureId) {
	static unsigned int debugFbo = -1, debugOutTex = -1, quadVBO = -1, quadVAO = -1;

	if(quadVAO == -1 && quadVBO == -1)
	{
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	if(debugFbo == -1 && debugOutTex == -1)
	{
    glGenFramebuffers(1, &debugFbo);
    glGenTextures(1, &debugOutTex);


		glBindTexture(GL_TEXTURE_2D, debugOutTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWitdh, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, debugFbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, debugOutTex, 0); // Set Output Tex
	glClear(GL_COLOR_BUFFER_BIT);

	float nearPlane = 0.1f;
	float farPlane = 2500.0f;

	debugShader->use();
	debugShader->setInt("image", 0);
	debugShader->setFloat("near_plane", nearPlane);
	debugShader->setFloat("far_plane", farPlane);

	// Set Input Tex
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	// Draw
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return debugOutTex;
}

void Game::Render() {
  int renderWitdh = CommancheRenderer::screenWidth;
  int renderHeight = CommancheRenderer::screenHeight;

	// Viewport
	static unsigned int vpFbo = -1, vpDepthAttachmentBuff = -1, vpOutTex = -1, vpDepthOutTex = -1;

	// Skybox
	static unsigned int skyboxVAO = -1, skyboxVBO = -1, skyboxCubeTextureId = -1;

	// Light
	//static unsigned int lightFbo = -1,lightDepthAttachmentBuff = -1, lightOutTex = -1;
	static unsigned int shadowMapFbo = -1, shadowMapOutTex = -1;

  if (vpFbo == -1) {
    glGenFramebuffers(1, &vpFbo);

    glGenTextures(1, &vpOutTex);
    glGenTextures(1, &vpDepthOutTex);

    glBindTexture(GL_TEXTURE_2D, vpOutTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWitdh, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glBindTexture(GL_TEXTURE_2D, vpDepthOutTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWitdh, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &vpDepthAttachmentBuff);
    glBindRenderbuffer(GL_RENDERBUFFER, vpDepthAttachmentBuff);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, renderWitdh, renderHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, vpFbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, vpDepthAttachmentBuff);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

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

	//if(lightFbo == -1 && lightOutTex == -1)
	//{
  //  glGenFramebuffers(1, &lightFbo);
  //  glGenTextures(1, &lightOutTex);


  //  glBindTexture(GL_TEXTURE_2D, lightOutTex);
  //  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWitdh, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //  glBindTexture(GL_TEXTURE_2D, 0);


  //  glGenRenderbuffers(1, &lightDepthAttachmentBuff);
  //  glBindRenderbuffer(GL_RENDERBUFFER, lightDepthAttachmentBuff);
  //  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, renderWitdh, renderHeight);

  //  glBindFramebuffer(GL_FRAMEBUFFER, lightFbo);
  //  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, lightDepthAttachmentBuff);
  //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//}

	const unsigned int SHADOW_WIDTH = renderWitdh, SHADOW_HEIGHT = renderHeight;
	if(shadowMapFbo == -1 && shadowMapOutTex == -1)
	{
    glGenFramebuffers(1, &shadowMapFbo);
    glGenTextures(1, &shadowMapOutTex);

		glBindTexture(GL_TEXTURE_2D, shadowMapOutTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapOutTex, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Render Scene From Light Perspective

	//glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glCullFace(GL_FRONT);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFbo);
  glClear(GL_DEPTH_BUFFER_BIT);

	glm::vec3 lookPos = camera->Position;
	float nearPlane = 0.1f;
	float farPlane = 3000.0f;
	float height = renderHeight * 4;
	float width = renderWitdh * 4;

	glm::mat4 orthoProjection = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, nearPlane, farPlane);
	glm::mat4 view = light->GetViewMatrix();

	shadowShader->use();
	RenderScene(shadowShader, orthoProjection ,view);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //glViewport(0, 0, renderWitdh, renderHeight);

	int debugTexture = DebugRenderOffScreenImage(SHADOW_WIDTH, SHADOW_HEIGHT, shadowMapOutTex);

	// Render Scene Lit

	//glViewport(0, 0, renderWitdh * 2, renderHeight * 2);
	glCullFace(GL_BACK);
  glBindFramebuffer(GL_FRAMEBUFFER, vpFbo);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vpOutTex, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, vpDepthOutTex, 0);
	GLenum drawBuffers2[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, drawBuffers2);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, renderWitdh, renderHeight);
  glClearColor(0.0f, 0.13f, 0.0, 1.0f);
  glEnable(GL_DEPTH_TEST);

  glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)renderWitdh / (float)renderHeight, 0.1f, 10000.0f);
	view = camera->GetViewMatrix();

	defaultShader->use();

	defaultShader->setInt("shadowMap", 0);
	defaultShader->setMat4("lightProjection", orthoProjection);
	defaultShader->setMat4("lightView", light->GetViewMatrix());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMapOutTex);

	RenderScene(defaultShader, projection, view);


  // Render Skybox

  glDepthFunc(GL_LEQUAL);

  skyboxShader->use();
  view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
  skyboxShader->setMat4("view", view);
  skyboxShader->setMat4("projection", projection);

  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeTextureId);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

	renderer->textureColorbuffer = vpOutTex;
	renderer->depthBuffer = vpDepthOutTex;
	//renderer->lightBuffer = lightOutTex;
	renderer->lightBuffer = debugTexture;
}

void Game::Destroy() { isRunning = false; }

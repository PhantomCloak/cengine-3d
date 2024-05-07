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
Shader *hdrShader;
Shader *kawaseUpsample;
Shader *kawaseDownsample;
Shader *bloomShader;

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

	light = CreateRef<DirectionalLight>(glm::vec3(0.2f), glm::vec3(0.3f),
                                      glm::vec3(1));
	light->Transform.position = glm::vec3(0, 3000, 0);
	light->Transform.rotation = glm::vec3(-66, 28, 0);
	//light->Transform.rotation = glm::vec3(1, 0, 0);

	light->Transform.scale = glm::vec3(10);

  defaultShader = new Shader("assets/shaders/default.vs", "assets/shaders/default.fs");
  skyboxShader = new Shader("assets/shaders/skybox.vs", "assets/shaders/skybox.fs");
  unlitShader = new Shader("assets/shaders/unlit.vs", "assets/shaders/unlit.fs");
  shadowShader = new Shader("assets/shaders/shadow.vs", "assets/shaders/shadow.fs");
  debugShader = new Shader("assets/shaders/debug.vs", "assets/shaders/debug.fs");
  hdrShader = new Shader("assets/shaders/hdr.vs", "assets/shaders/hdr.fs");
  bloomShader = new Shader("assets/shaders/bloom.vs", "assets/shaders/bloom.fs");

  kawaseUpsample = new Shader("assets/shaders/kawase.vs", "assets/shaders/kawase_upsample.fs");
  kawaseDownsample = new Shader("assets/shaders/kawase.vs", "assets/shaders/kawase_downsample.fs");

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

bool updateShadows = false;
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
	else if(Keyboard::IsKeyPressing(Key_F))
	{
		updateShadows = true;
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

struct BlurTexture {
	unsigned int TextureId;
	int Width;
	int Height;
};

int TurboBlur(int renderWidth, int renderHeight, int brightTexture, int sampleCount, BlurTexture* blurTex, int sceneTexture) {
	static unsigned int quadVAO = -1, quadVBO = -1;
	static unsigned int hdrFbo = -1, hdrOutTex = -1;
	static unsigned int bloomFbo = -1, bloomOutTex = -1;

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

	if(hdrFbo == -1)
	{
    glGenFramebuffers(1, &hdrFbo);
    glGenTextures(1, &hdrOutTex);


		glBindTexture(GL_TEXTURE_2D, hdrOutTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderWidth, renderHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &bloomFbo);
    glGenTextures(1, &bloomOutTex);

		glBindTexture(GL_TEXTURE_2D, bloomOutTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderWidth, renderHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}


	glBindFramebuffer(GL_FRAMEBUFFER, hdrFbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrOutTex, 0); // Set Output Tex
	glClear(GL_COLOR_BUFFER_BIT);


	kawaseDownsample->use();
	kawaseDownsample->setInt("u_sourceTex", 0);
	kawaseDownsample->setVec2("u_sourceRes", glm::vec2(renderWidth, renderHeight));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brightTexture);
	glBindVertexArray(quadVAO);


	// Downscale
	for(int i = 0; i < sampleCount; i++)
	{
		BlurTexture blurSample = blurTex[i];
		glViewport(0, 0, blurSample.Width, blurSample.Height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurSample.TextureId, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		kawaseDownsample->setVec2("u_sourceRes", glm::vec2(blurSample.Width, blurSample.Height));
		glBindTexture(GL_TEXTURE_2D, blurSample.TextureId);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	kawaseUpsample->use();

	glActiveTexture(GL_TEXTURE0);
	kawaseUpsample->setInt("u_sourceTex", 0);

	for(int i = sampleCount -1; i > 0; i--)
	{
		BlurTexture blurSample = blurTex[i];
		BlurTexture nextBlurSample = blurTex[i - 1];

		glBindTexture(GL_TEXTURE_2D, blurSample.TextureId);
		glViewport(0, 0, nextBlurSample.Width, nextBlurSample.Height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextBlurSample.TextureId, 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, bloomFbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomOutTex, 0); // Set Output Tex

	bloomShader->use();
	bloomShader->setInt("scene", 0);
	bloomShader->setInt("bloomBlur", 1);

	//bloomShader->setFloat("exposure", Editor::Instance->entityInspector->exposure);
	bloomShader->setFloat("exposure", Editor::Instance->ppfxInspector->exposure);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, blurTex[0].TextureId);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	return bloomOutTex;
}

int RenderDepthToTexture(int renderWitdh, int renderHeight, int textureId) {
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
	float farPlane = 4000.0f;

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
	static unsigned int vpFbo = -1, vpDepthRbo = -1, vpOutTex = -1, vpDepthOutTex = -1, vpBrightOutTex = -1;

	// Skybox
	static unsigned int skyboxVAO = -1, skyboxVBO = -1, skyboxCubeTextureId = -1;

	// Light
	static unsigned int shadowMapFbo = -1, shadowMapOutTex = -1;
	static bool shadowInitial = false;

	// Bloom
	const int sampleCount = 8;
	static BlurTexture blurChain[sampleCount];

  if (vpFbo == -1) {
    glGenFramebuffers(1, &vpFbo);

    glGenTextures(1, &vpOutTex);
    glGenTextures(1, &vpDepthOutTex);
    glGenTextures(1, &vpBrightOutTex);

    glBindTexture(GL_TEXTURE_2D, vpOutTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWitdh, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, vpBrightOutTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderWitdh, renderHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
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

    glGenRenderbuffers(1, &vpDepthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, vpDepthRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, renderWitdh, renderHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, vpFbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, vpDepthRbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// HACK
		for(int i = 0; i < sampleCount; i++)
		{
			int textureW = renderWitdh / pow(2, i);
			int textureH = renderHeight / pow(2, i);


			unsigned int texture = -1;
			glGenTextures(1, &texture);

			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureW, textureH, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			blurChain[i] = BlurTexture({
					.TextureId = texture,
					.Width = textureW,
					.Height = textureH
			});
		}
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

	const unsigned int SHADOW_WIDTH = 1024 * 5, SHADOW_HEIGHT = 1024 * 5;
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

	glm::mat4 view;

	float nearPlane = 0.1f;
	float farPlane = 4000.0f;

	float height = SHADOW_HEIGHT;
	float width = SHADOW_WIDTH;

	glm::mat4 orthoProjection = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, nearPlane, farPlane);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	if(!shadowInitial || updateShadows)
	{
		glCullFace(GL_FRONT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::vec3 lookPos = camera->Position;
		view = light->GetViewMatrix();

		shadowShader->use();
		RenderScene(shadowShader, orthoProjection ,view);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		shadowInitial = true;
		updateShadows = false;
	}

	view = light->GetViewMatrix();

	int debugTexture = RenderDepthToTexture(SHADOW_WIDTH, SHADOW_HEIGHT, shadowMapOutTex);

	// Render Scene Lit

	glCullFace(GL_BACK);
  glBindFramebuffer(GL_FRAMEBUFFER, vpFbo);
	glViewport(0, 0, renderWitdh, renderHeight);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vpOutTex, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, vpDepthOutTex, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, vpBrightOutTex, 0);

	GLenum drawBuffers2[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, drawBuffers2);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glViewport(0, 0, renderWitdh, renderHeight);
  glClearColor(0.0f, 0.13f, 0.0, 1.0f);
  glEnable(GL_DEPTH_TEST);

  glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)renderWitdh / (float)renderHeight, 0.1f, 10000.0f);
	view = camera->GetViewMatrix();

	defaultShader->use();

	defaultShader->setInt("shadowMap", 0);
	defaultShader->setMat4("lightProjection", orthoProjection);
	defaultShader->setMat4("lightView", light->GetViewMatrix());
	defaultShader->setVec3("brightCutoff", Editor::Instance->ppfxInspector->brightColorCuttoff);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMapOutTex);

	RenderScene(defaultShader, projection, view);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int debugHdrTexture = TurboBlur(renderWitdh, renderHeight, vpBrightOutTex, sampleCount, blurChain, vpOutTex);

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
	renderer->lightBuffer = debugTexture;
	renderer->hdrBuffer = debugHdrTexture;
}

void Game::Destroy() { isRunning = false; }

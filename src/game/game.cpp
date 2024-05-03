#include "game.h"
#include "../assetmgr/AssetManager.h"
#include "../common/common.h"
#include "../io/cursor.h"
#include "../io/keyboard.h"
#include "log/log.h"
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "constants.h"
#include "render/Camera.h"
#include "render/backends/opengl/DirectonalLight.h"
#include "render/backends/opengl/Model.h"
#include "render/backends/opengl/Shader.h"

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
Shader *gShader;
Shader *lightShader;

Ref<Model> sampleModel;
Ref<Model> sampleModel2;
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
  // light->Transform.rotation = glm::vec3(1, 0, 0);

  light->Transform.scale = glm::vec3(10);

  defaultShader =
      new Shader("assets/shaders/default.vs", "assets/shaders/default.fs");
  skyboxShader =
      new Shader("assets/shaders/skybox.vs", "assets/shaders/skybox.fs");
  unlitShader =
      new Shader("assets/shaders/unlit.vs", "assets/shaders/unlit.fs");
  shadowShader =
      new Shader("assets/shaders/shadow.vs", "assets/shaders/shadow.fs");
  debugShader =
      new Shader("assets/shaders/debug.vs", "assets/shaders/debug.fs");
  hdrShader = new Shader("assets/shaders/hdr.vs", "assets/shaders/hdr.fs");
  bloomShader =
      new Shader("assets/shaders/bloom.vs", "assets/shaders/bloom.fs");
  gShader =
      new Shader("assets/shaders/gshader.vs", "assets/shaders/gshader.fs");
  lightShader =
      new Shader("assets/shaders/light.vs", "assets/shaders/light.fs");

  kawaseUpsample = new Shader("assets/shaders/kawase.vs",
                              "assets/shaders/kawase_upsample.fs");
  kawaseDownsample = new Shader("assets/shaders/kawase.vs",
                                "assets/shaders/kawase_downsample.fs");

  sampleModel = CreateRef<Model>("assets/models/sponza.obj");
  // sampleModel->Transform.scale = glm::vec3(0.5f);
  sampleModel2 = CreateRef<Model>("assets/models/backpack.obj");

  lightModel = CreateRef<Model>("assets/models/cube.obj");

  Root->AddChild(sampleModel);
  Root->AddChild(sampleModel2);
  Root->AddChild(light);
  Root->AddChild(lightModel);
	Cursor::CaptureMouse(true);
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
  const float mulSpeed = 0.1f;

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
    // camera->Yaw += 1.0f;
    camera->updateCameraVectors();
  } else if (Keyboard::IsKeyPressing(Key_E)) {
    // camera->Yaw -= 1.0f;
    yaw -= 1.0f;
    camera->updateCameraVectors();
  } else if (Keyboard::IsKeyPressing(Key_H)) {
    pitch += 1.0f;

  } else if (Keyboard::IsKeyPressing(Key_J)) {
    pitch -= 1.0f;
  } else if (Keyboard::IsKeyPressing(Key_F)) {
    updateShadows = true;
  }

  //if (Editor::Instance->viewport->IsFocused())
    camera->ProcessMouseMovement(cposOffset.x, cposOffset.y);

  lastCpos = cpos;
  Keyboard::Poll();
}

void RenderScene(Shader *shader, glm::mat4 projectionMat, glm::mat4 viewMat) {
  shader->setVec3("viewPos", camera->Position);

  light->Draw(*shader);

  shader->setFloat("material.shininess", 64.0f);

  shader->setMat4("projection", projectionMat);
  shader->setMat4("view", viewMat);

  sampleModel2->Draw(*shader);
  sampleModel->Draw(*shader);

  lightModel->Transform = light->Transform;
  lightModel->Draw(*shader);
}

struct BlurTexture {
  unsigned int TextureId;
  int Width;
  int Height;
};

int TurboBlur(int renderWidth, int renderHeight, int brightTexture,
              int sampleCount, BlurTexture *blurTex, int sceneTexture) {
  static unsigned int quadVAO = -1, quadVBO = -1;
  static unsigned int hdrFbo = -1, hdrOutTex = -1;
  static unsigned int bloomFbo = -1, bloomOutTex = -1;

  if (quadVAO == -1 && quadVBO == -1) {
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
  }

  if (hdrFbo == -1) {
    glGenFramebuffers(1, &hdrFbo);
    glGenTextures(1, &hdrOutTex);

    glBindTexture(GL_TEXTURE_2D, hdrOutTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderWidth, renderHeight, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &bloomFbo);
    glGenTextures(1, &bloomOutTex);

    glBindTexture(GL_TEXTURE_2D, bloomOutTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderWidth, renderHeight, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, hdrFbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         hdrOutTex, 0); // Set Output Tex
  glClear(GL_COLOR_BUFFER_BIT);

  kawaseDownsample->use();
  kawaseDownsample->setInt("u_sourceTex", 0);
  kawaseDownsample->setVec2("u_sourceRes",
                            glm::vec2(renderWidth, renderHeight));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, brightTexture);
  glBindVertexArray(quadVAO);

  // Downscale
  for (int i = 0; i < sampleCount; i++) {
    BlurTexture blurSample = blurTex[i];
    glViewport(0, 0, blurSample.Width, blurSample.Height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           blurSample.TextureId, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    kawaseDownsample->setVec2("u_sourceRes",
                              glm::vec2(blurSample.Width, blurSample.Height));
    glBindTexture(GL_TEXTURE_2D, blurSample.TextureId);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glBlendEquation(GL_FUNC_ADD);

  kawaseUpsample->use();

  glActiveTexture(GL_TEXTURE0);
  kawaseUpsample->setInt("u_sourceTex", 0);

  for (int i = sampleCount - 1; i > 0; i--) {
    BlurTexture blurSample = blurTex[i];
    BlurTexture nextBlurSample = blurTex[i - 1];

    glBindTexture(GL_TEXTURE_2D, blurSample.TextureId);
    glViewport(0, 0, nextBlurSample.Width, nextBlurSample.Height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           nextBlurSample.TextureId, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  glDisable(GL_BLEND);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, bloomFbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         bloomOutTex, 0); // Set Output Tex

  bloomShader->use();
  bloomShader->setInt("scene", 0);
  bloomShader->setInt("bloomBlur", 1);
//  bloomShader->setFloat("exposure",
//                        Editor::Instance->entityInspector->exposure);
//
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
  static unsigned int debugFbo = -1, debugOutTex = -1, quadVBO = -1,
                      quadVAO = -1;

  if (quadVAO == -1 && quadVBO == -1) {
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
  }

  if (debugFbo == -1 && debugOutTex == -1) {
    glGenFramebuffers(1, &debugFbo);
    glGenTextures(1, &debugOutTex);

    glBindTexture(GL_TEXTURE_2D, debugOutTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWitdh, renderHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, debugFbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         debugOutTex, 0); // Set Output Tex
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

void Skybox(glm::mat4 projection) {
  // Skybox
  static unsigned int skyboxVAO = -1, skyboxVBO = -1, skyboxCubeTextureId = -1;

  if (skyboxVBO == -1 && skyboxVBO == -1 && skyboxCubeTextureId == -1) {
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);

    std::vector<std::string> faces{"skybox/right.jpg", "skybox/left.jpg",
                                   "skybox/top.jpg",   "skybox/bottom.jpg",
                                   "skybox/front.jpg", "skybox/back.jpg"};
    skyboxCubeTextureId = AssetManager::LoadCubeMap(faces);
  }

  glDepthFunc(GL_LEQUAL);

  skyboxShader->use();
  glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
  skyboxShader->setMat4("view", view);
  skyboxShader->setMat4("projection", projection);

  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeTextureId);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int RenderDef(int renderWitdh, int renderHeight, int gPosTex, int gNormalTex, int gAlbedoTex, glm::mat4 projection, glm::mat4 view, int gBuffer) {

  //static unsigned int lightBufferFbo = -1, lightOutTex, lightDepthRbo;
  static unsigned int quadVAO = -1, quadVBO = -1;

  static std::vector<glm::vec3> lightPositions;
  static std::vector<glm::vec3> lightColors;

  if (quadVAO == -1 && quadVBO == -1) {
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));

	const unsigned int NR_LIGHTS = 16;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
		float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
		float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
		float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
		float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}
  }

//  if (lightBufferFbo == -1) {
//    glGenFramebuffers(1, &lightBufferFbo);
//    glGenTextures(1, &lightOutTex);
//
//    glBindTexture(GL_TEXTURE_2D, lightOutTex);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderWitdh, renderHeight, 0,
//                 GL_RGBA, GL_FLOAT, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//    glGenRenderbuffers(1, &lightDepthRbo);
//    glBindRenderbuffer(GL_RENDERBUFFER, lightDepthRbo);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, renderWitdh,
//                          renderHeight);
//
//    //glBindFramebuffer(GL_FRAMEBUFFER, lightBufferFbo);
//    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
//    //                          GL_RENDERBUFFER, lightDepthRbo);
//    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//
//
//  }
//
  //glBindFramebuffer(GL_FRAMEBUFFER, lightBufferFbo);
  //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightOutTex, 0); // Set Output Tex

  //glClear(GL_COLOR_BUFFER_BIT);


  lightShader->use();
  //lightShader->setMat4("projection", projection);
  //lightShader->setMat4("view", view);
  
  lightShader->setInt("gPosition", 0);
  lightShader->setInt("gNormal", 1);
  lightShader->setInt("gAlbedoSpec", 2);
	lightShader->setVec3("viewPos", camera->Position);

  for (unsigned int i = 0; i < lightPositions.size(); i++)
  {
	  lightShader->setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
	  lightShader->setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
	  // update attenuation parameters and calculate radius
	  const float linear = 0.7f; const float quadratic = 1.8f;
	  lightShader->setFloat("lights[" + std::to_string(i) + "].Linear", linear);
	  lightShader->setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
  }

  // Set Input Tex
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, gPosTex);


  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, gNormalTex);


  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, gAlbedoTex);

  // Draw
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, renderWitdh, renderHeight, 0, 0, renderWitdh, renderHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);



  //glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //return lightOutTex;
}

void Game::Render() {
  int renderWitdh = CommancheRenderer::screenWidth;
  int renderHeight = CommancheRenderer::screenHeight;

  // Viewport
  static unsigned int vpFbo = -1, vpDepthRbo = -1, vpOutTex = -1,
                      vpDepthOutTex = -1, vpBrightOutTex = -1;

  // G-Stuff
  static unsigned int gBufferFbo = -1, gPosition, gNormal, gAlbedoSpec,
                      gDepthRbo;
  if (gBufferFbo == -1) {
    glGenFramebuffers(1, &gBufferFbo);

    // - position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderWitdh, renderHeight, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // - normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, renderWitdh, renderHeight, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // - color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderWitdh, renderHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenRenderbuffers(1, &gDepthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, gDepthRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, renderWitdh,
                          renderHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, gBufferFbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, gDepthRbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
  }

  glm::mat4 projection = glm::perspective(
      glm::radians(camera->Zoom), (float)renderWitdh / (float)renderHeight,
      0.1f, 10000.0f);
  glm::mat4 view = camera->GetViewMatrix();

  // G-BUFFER
  // =====================================================================

  glViewport(0, 0, renderWitdh, renderHeight);
  glBindFramebuffer(GL_FRAMEBUFFER, gBufferFbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gShader->use();

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gPosition, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormal, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         gAlbedoSpec, 0);

  GLenum drawBuffers2[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                           GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, drawBuffers2);

  RenderScene(gShader, projection, view);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  int text = RenderDef(renderWitdh, renderHeight, gPosition, gNormal, gAlbedoSpec, projection, view, gBufferFbo);

  //renderer->textureColorbuffer = vpOutTex;
  //renderer->depthBuffer = gAlbedoSpec;
  //renderer->lightBuffer = gNormal;
  //renderer->hdrBuffer = text;
}

void Game::Destroy() { isRunning = false; }

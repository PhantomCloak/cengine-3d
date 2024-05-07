#pragma once

#include "../log/log.h"
#include "glm/glm.hpp"
#include "render_primitives.h"
#include <string>

typedef void (*OnViewportChange)(int, int);

class CommancheRenderer {
    public:
    void* wnd;
    void* gctx;
    std::string driverStr;
    void Initialize(const std::string& title, int windowHeight, int windowWidth);
    int GetFrame();
    CommancheTextureInfo GetTextureInfo(int id);
    int LoadFont(const std::string& path, int size);
    void OffsetCamera(float vertical, float horizontal);
    void SetCameraZoom(float zoom);
    void RenderStart();
    void RenderEnd();
    void Destroy();
    void BeginDraw();
    void EndDraw();
    float GetFps();

		void AddOnViewportChangeEvent(OnViewportChange event);
    static glm::mat4 ProjectionMat;
    glm::mat4 view;
    static int screenWidth;
    static int screenHeight;
    static CommancheRenderer* Instance;
    CommancheRenderTexture viewTexture;
    bool isTextureModeEnabled = false;
    void* WndPtr;
		
		int textureId;

    unsigned int framebuffer, textureColorbuffer, depthBuffer, lightBuffer, hdrBuffer;
    private:
		std::vector<OnViewportChange> _callbackList;
};

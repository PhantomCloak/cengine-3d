#pragma once

#include "../log/log.h"
#include "glm/glm.hpp"
#include "render_primitives.h"
#include <string>

typedef void (*OnViewportChange)(int, int);

class CommancheRenderer {
    public:
    void* WndPtr;
    void Initialize(const std::string& title, int windowHeight, int windowWidth);
    int GetFrame();
    void RenderStart();
    void RenderEnd();
    void Destroy();
    void BeginDraw();
    void EndDraw();

		void AddOnViewportChangeEvent(OnViewportChange event);
    static glm::mat4 ProjectionMat;
    glm::mat4 view;
    static int ScreenWidth;
    static int ScreenHeight;
    static CommancheRenderer* Instance;
    CommancheRenderTexture viewTexture;
    bool isTextureModeEnabled = false;
		
		int textureId;

    unsigned int framebuffer, textureColorbuffer, depthBuffer, lightBuffer, hdrBuffer;
    private:
		std::vector<OnViewportChange> _callbackList;
};

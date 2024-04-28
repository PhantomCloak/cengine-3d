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
    void InitializeShaders(const std::string& defaultShaderPath);

    void SetPPM(int val);
    bool IsTextureValid(int textureId);
    bool IsShaderValid(int shaderId);
    void DrawRectRangle(float x, float y, float width, float height, float rotation, CommancheColorRGBA color = { 255, 255, 255, 255 });
    void CDrawImage(int textureId, float x, float y, float width, float height, float rotation, float srcX, float srcY, float srcWidth, float srcHeight, CommancheColorRGBA color);
    void CDrawText(int fontId, std::string message, float x, float y, float size, CommancheColorRGB color = { 255, 255, 255 });
    void CDrawLine(float startx, float starty, float endx, float endy, CommancheColorRGB color = { 255, 255, 255 });
    int GetFrame();
    int CLoadTexture(const std::string& path);
    int CLoadShader(const std::string& path, const std::string shaderName);
    void UpdateRenderTexture(glm::vec2 size);
    CommancheTextureInfo GetTextureInfo(int id);
    int LoadFont(const std::string& path, int size);
    void OffsetCamera(float vertical, float horizontal);
    void SetCameraZoom(float zoom);
    void DrawGrids();
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
    CommancheCamera camX = { 0 };
		
		
		int textureId;

    unsigned int framebuffer, textureColorbuffer, depthBuffer, lightBuffer;
    private:
		std::vector<OnViewportChange> _callbackList;
};

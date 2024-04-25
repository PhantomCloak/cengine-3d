#include <glad/glad.h>
#include "../../../core/coordinate_system.h"
#include "../../render.h"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <ft2build.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include FT_FREETYPE_H

GLFWwindow* k_window;
std::string driverStr;
glm::mat4 CommancheRenderer::ProjectionMat;
int CommancheRenderer::screenWidth;
int CommancheRenderer::screenHeight;

int nextFontId = 0;
const int scaleFactor = 1;

unsigned int fboDefaultRenderPass = 0;

CommancheRenderer* CommancheRenderer::Instance;

extern "C" double getScreenScaleFactor();


void CommancheRenderer::AddOnViewportChangeEvent(OnViewportChange event) {
    _callbackList.push_back(event);
}

int CommancheRenderer::LoadFont(const std::string& path, int fontSize) {

    return -1;
}

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    long Advance;           // Offset to advance to next glyph
};


void InitFreeType() {
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    CommancheRenderer::screenWidth = width;
    CommancheRenderer::screenHeight = height;
    glViewport(0, 0, width, height);
}


float CommancheRenderer::GetFps() {
    return 0;
}

void CommancheRenderer::SetCameraZoom(float zoom) {
    camX.SetCameraZoom(zoom);
}

void CommancheRenderer::OffsetCamera(float vertical, float horizontal) {
    camX.OffsetCamera(vertical, horizontal);
}

void CommancheRenderer::Initialize(const std::string& title, int windowWidth, int windowHeight) {
    Instance = this;
    glfwInit();

    std::string titleStr = title + " - Backend [OpenGL]";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    k_window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);

    wnd = k_window;
    if (k_window == NULL) {
        Log::Err("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(k_window);
    gladLoadGL();
    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(
    k_window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);

        for (auto callback : CommancheRenderer::Instance->_callbackList) {
            callback(width, height);
        }
    });

    camX.SetTarget({ 0, 1920, 1080, 0 });
    screenWidth = windowWidth;
    screenHeight = windowHeight;

    glfwSwapInterval(0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    glGenFramebuffers(1, &fboDefaultRenderPass);
}

void CommancheRenderer::InitializeShaders(const std::string& defaultShaderPath) {
}


void CommancheRenderer::DrawRectRangle(float x, float y, float width, float height, float rotation, CommancheColorRGBA color) {
}

void CommancheRenderer::CDrawImage(int textureId, float x, float y, float width, float height, float rotation, float srcX, float srcY, float srcWidth, float srcHeight, CommancheColorRGBA color) {
}

bool CommancheRenderer::IsShaderValid(int shaderId) {
}

bool CommancheRenderer::IsTextureValid(int textureId) {
}

int GenerateTextureFromText(std::string text, int fontId, CommancheColorRGB textColor) {
    return -1;
}

void CommancheRenderer::CDrawLine(float startx, float starty, float endx, float endy, CommancheColorRGB color){

};

void CommancheRenderer::CDrawText(int fontId, std::string message, float x, float y, float size, CommancheColorRGB color) {
}


int CommancheRenderer::CLoadShader(const std::string& path, const std::string shaderName) {
}

int CommancheRenderer::GetFrame() {
    return textureColorbuffer;
}

void CommancheRenderer::UpdateRenderTexture(glm::vec2 size) {
}

int CommancheRenderer::CLoadTexture(const std::string& path) {
}

void CommancheRenderer::DrawGrids() {
}


CommancheTextureInfo CommancheRenderer::GetTextureInfo(int id) {
    // Texture texture = glTextures[id];

    // CommancheTextureInfo inf;
    // inf.width = texture.width;
    // inf.height = texture.height;

    // return inf;
}

void CommancheRenderer::RenderStart() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.53, 0.80f, 0.921f, 1.0f);
}

void CommancheRenderer::BeginDraw() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void CommancheRenderer::RenderEnd() {
    glfwSwapBuffers(k_window);
    glfwPollEvents();
}

void CommancheRenderer::EndDraw() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CommancheRenderer::Destroy() {
    glfwDestroyWindow(k_window);
    glfwTerminate();
}

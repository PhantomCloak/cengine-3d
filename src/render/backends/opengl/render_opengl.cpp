#include <glad/glad.h>
#include "../../../core/coordinate_system.h"
#include "../../render.h"
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

GLFWwindow* k_window;
std::string driverStr;
glm::mat4 CommancheRenderer::ProjectionMat;
int CommancheRenderer::ScreenWidth;
int CommancheRenderer::ScreenHeight;

int nextFontId = 0;
const int scaleFactor = 1;

unsigned int fboDefaultRenderPass = 0;

CommancheRenderer* CommancheRenderer::Instance;

extern "C" double getScreenScaleFactor();

void CommancheRenderer::AddOnViewportChangeEvent(OnViewportChange event) {
    _callbackList.push_back(event);
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

    WndPtr = k_window;
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

    ScreenWidth = windowWidth;
    ScreenHeight = windowHeight;

    glfwSwapInterval(0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenFramebuffers(1, &fboDefaultRenderPass);
		glfwMaximizeWindow(k_window);
}

int CommancheRenderer::GetFrame() {
    return textureColorbuffer;
}

void CommancheRenderer::RenderStart() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.53f, 0.70f, 0.921f, 1.0f);

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

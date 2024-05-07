#include "../../../render/render.h"
#include "../../cursor.h"
#include "../../../core/coordinate_system.h"
#include <GLFW/glfw3.h>

#if EDITOR
#include "../../../editor/editor.h"
#endif

static GLFWwindow* wnd;

void Cursor::Setup(void* window) {
    wnd = (GLFWwindow*)window;
}

glm::vec2 Cursor::GetCursorPosition() {
    double x = 0;
    double y = 0;

    glfwGetCursorPos(wnd, &x, &y);
    return glm::vec2(x, y);
}

glm::vec2 Cursor::GetCursorWorldPosition(glm::vec2 screenPoint, CommancheCamera cam) {

}

bool Cursor::HasLeftCursorClicked() {
    return glfwGetMouseButton(wnd, 0);
}

bool Cursor::HasRightCursorClicked() {
    return glfwGetMouseButton(wnd, 1);
}

void Cursor::CaptureMouse(bool shouldCapture)
{
	if(shouldCapture)
		glfwSetInputMode((GLFWwindow*)CommancheRenderer::Instance->wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode((GLFWwindow*)CommancheRenderer::Instance->wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

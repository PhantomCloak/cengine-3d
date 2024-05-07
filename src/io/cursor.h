#pragma once

#include "../render/render_primitives.h"
#include <glm/glm.hpp>

class Camera;
class Cursor {
public:
    static void Setup(void* window);
    static glm::vec2 GetCursorPosition();
    static glm::vec2 GetCursorWorldPosition(glm::vec2 screenPoint, Camera cam);
    static bool HasLeftCursorClicked();
    static bool HasRightCursorClicked();
		static void CaptureMouse(bool shouldCapture);
};

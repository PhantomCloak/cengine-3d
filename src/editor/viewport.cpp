#include "viewport.h"
#include "../io/cursor.h"
#include "../render/render.h"
#include "imgui.h"
#include <algorithm>
#include <ApplicationServices/ApplicationServices.h>

void Fit(int image, int width, int height, bool center = false) {
    ImVec2 area = ImGui::GetContentRegionAvail();

    float scale = area.x / width;

    float y = height * scale;
    if (y > area.y) {
        scale = area.y / height;
    }

    int sizeX = int(width * scale);
    int sizeY = int(height * scale);

    if (center) {
        ImGui::SetCursorPosX(0);
        ImGui::SetCursorPosX(area.x / 2 - sizeX / 2);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (area.y / 2 - sizeY / 2));
    }

    CommancheRect sourceRect = CommancheRect{ 0, 0, float(width), -float(height) };
    float destWidth = float(sizeX);
    float destHeight = float(sizeY);
    ImVec2 uv0;
    ImVec2 uv1;

    if (sourceRect.width < 0) {
        uv0.x = -((float)sourceRect.x / width);
        uv1.x = (uv0.x - (float)(fabs(sourceRect.width) / width));
    } else {
        uv0.x = (float)sourceRect.x / width;
        uv1.x = uv0.x + (float)(sourceRect.width / width);
    }

    if (sourceRect.height < 0) {
        uv0.y = -((float)sourceRect.y / height);
        uv1.y = (uv0.y - (float)(fabs(sourceRect.height) / height));
    } else {
        uv0.y = (float)sourceRect.y / height;
        uv1.y = uv0.y + (float)(sourceRect.height / height);
    }

    static int imgIdx = 0;

    imgIdx = image;

		ImGui::Image((void*)imgIdx, ImVec2(float(destWidth), float(destHeight)), uv0, uv1);
}

bool EditorViewPort::IsFocused() {
    return isFocused;
}


#include <algorithm> // Include this for std::min and std::max

void Fit2(int image, int srcWidth, int srcHeight) {
    ImVec2 area = ImGui::GetWindowSize(); // Use the entire window size.

    // Determine scaling factor to fit the image within the window while maintaining aspect ratio.
    float scale = std::min(area.x / srcWidth, area.y / srcHeight);

    // Calculate the target size of the image based on the scaling factor.
    int sizeX = static_cast<int>(srcWidth * scale);
    int sizeY = static_cast<int>(srcHeight * scale);

    // Calculate the centering position.
    float posX = (area.x - sizeX) / 2.0f;
    float posY = (area.y - sizeY) / 2.0f;

    // Set the cursor position for centered rendering.
    ImGui::SetCursorPosX(posX);
    ImGui::SetCursorPosY(posY);

    // Set up UV coordinates. Since image is upside down, we swap top and bottom UV coordinates.
    ImVec2 uv0(0.0f, 1.0f); // Start from the bottom-left corner of the image.
    ImVec2 uv1(1.0f, 0.0f); // End at the top-right corner of the image.

    // Render the image with corrected UVs and resized dimensions.
    ImGui::Image((void*)(intptr_t)image, ImVec2(float(sizeX), float(sizeY)), uv0, uv1);

		// Optionally draw black bars if needed (you can adjust the color and styling as necessary)
		ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Black color for the letterbox
}

void EditorViewPort::RenderWindow() {
	static bool Open = true;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	std::string title = "2D View " + std::to_string((int)ViewportSize.x) + "x" + std::to_string((int)ViewportSize.y) + "###2DView";
	if (ImGui::Begin(title.c_str(), &Open, ImGuiWindowFlags_NoNav)) {

		static float zoomValue = 1;
		static float lastZoomValue = 1;

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 contentAvail = ImGui::GetContentRegionAvail();

		ImGuiIO& io = ImGui::GetIO();

		if ((ImGui::IsWindowHovered() || ImGui::IsWindowFocused()) && *io.MouseClicked != 0) {

				isFocused = true;
				Cursor::CaptureMouse(true);
		}        
		

		if (isFocused && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Escape)))
		{
			isFocused = false;
			Cursor::CaptureMouse(false);
		}

		int frameId = CommancheRenderer::Instance->GetFrame();
		Fit2(frameId, 1920, 1080);
	}
	ImGui::End();

	if (ImGui::Begin("Depth Buffer", &Open, ImGuiWindowFlags_NoNav)) { 
		int frameId = CommancheRenderer::Instance->depthBuffer;
		Fit2(frameId, 1920, 1080);
	}
	ImGui::End();

	ImGui::PopStyleVar();
}

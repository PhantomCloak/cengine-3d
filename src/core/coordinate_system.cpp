#pragma once

#include "coordinate_system.h"
#include "../editor/editor.h"
#include "../render/render.h"

int CoordinateCalculator::referenceWidth = 1920;
int CoordinateCalculator::referenceHeight = 1080;
float CoordinateCalculator::meterPixelRatioX = 100;   // X: 1 meter = 100 pixels at 1920x1080
float CoordinateCalculator::meterPixelRatioY = 56.25; // Y: 1 meter = 56.25 pixels at 1920x1080

void CoordinateCalculator::ConvertMetersToPixels(float& xMeter, float& yMeter) {
#if EDITOR
    float scale = 1;

    float scaleX = (Editor::Instance->GetViewPortSize().x * scale) / static_cast<float>(referenceWidth);
    float scaleY = (Editor::Instance->GetViewPortSize().y * scale) / static_cast<float>(referenceHeight);

    xMeter = xMeter * (referenceWidth / meterPixelRatioX) * scaleX;
    yMeter = yMeter * (referenceHeight / meterPixelRatioY) * scaleY;
#endif
}

void CoordinateCalculator::ConvertPixelsToMeters(float& xPixel, float& yPixel) {
#if EDITOR
    float scale = 1;
    
    float scaleX = (Editor::Instance->GetViewPortSize().x * scale) / static_cast<float>(referenceWidth);
    float scaleY = (Editor::Instance->GetViewPortSize().y * scale) / static_cast<float>(referenceHeight);

    xPixel = (xPixel / scaleX) / (referenceWidth / meterPixelRatioX);
    yPixel = (yPixel / scaleY) / (referenceHeight / meterPixelRatioY);
#endif
}

void CoordinateCalculator::NormalizePixelCoord(float& xPixel, float& yPixel) {
#if EDITOR
    float scaleX = CommancheRenderer::ScreenWidth / static_cast<float>(referenceWidth);
    float scaleY = CommancheRenderer::ScreenHeight / static_cast<float>(referenceHeight);

    xPixel = xPixel / scaleX;
    yPixel = yPixel / scaleY;
#endif
}

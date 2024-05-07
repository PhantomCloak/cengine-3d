#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


struct CommancheRect {
    float x;
    float y;
    float height;
    float width;

    CommancheRect(float x = 0, float y = 0, float width = 0, float height = 0) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }
};

struct ViewRect {
    float left;
    float right;
    float top;
    float bottom;
};

typedef unsigned int CommancheRenderTexture;

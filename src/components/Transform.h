#pragma once

#include <glm/glm.hpp>

struct TransformC{
  glm::vec3 position;
  glm::vec3 scale;
  glm::vec3 rotation;

  TransformC(glm::vec3 pos = glm::vec3(0), glm::vec3 scale = glm::vec3(1), glm::vec3 rotation = glm::vec3(0))
  {
    this->position = pos;
    this->scale = scale;
    this->rotation = rotation;
  };
};

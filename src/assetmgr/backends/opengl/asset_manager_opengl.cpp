#include "../../../log/log.h"
#include "../../AssetManager.h"
#include <glm/glm.hpp>
#include "stb_image.h"
#include <glad/glad.h>
#include <iostream>

std::map<std::string, int> textures;
std::map<std::string, int> shaders;
std::map<std::string, int> fonts;

CommancheRenderer* crender;

void AssetManager::Initialize(CommancheRenderer* render) {
    crender = render;
    Log::Inf("Asset manager initialized");
}


int AssetManager::AddTexture(const std::string& assetId, const std::string& path) {
    //int textureId = crender->CLoadTexture(path);

    //if (!crender->IsTextureValid(textureId)) {
    //    Log::Warn("Texture with invalid ID %d has tried to load", textureId);
    //    return -1;
    //}

    //CommancheTextureInfo inf = crender->GetTextureInfo(textureId);
    //Log::Inf("Texture has loaded id: %d size: %dx%d", textureId, inf.width, inf.height);
    //textures.insert(std::make_pair(assetId, textureId));
    //return textureId;
}

void AssetManager::AddFont(const std::string& assetId, const std::string& path, int fontSize) {
    // int fontId = renderer->LoadFont(path, fontSize);
    // fonts.emplace(std::make_pair(assetId, fontId));
}

std::vector<std::string> AssetManager::GetLoadedTextures() {
    std::vector<std::string> loadedTextures;
    for (std::map<std::string, int>::iterator it = textures.begin(); it != textures.end(); ++it) {
        loadedTextures.push_back(it->first);
    }

    return loadedTextures;
}

int AssetManager::GetFont(const std::string& assetId) {

    if (fonts.find(assetId) == fonts.end()) {
        Log::Err("font: %s couldn't found in the registry", assetId.c_str());
        return -1;
    }

    return fonts.at(assetId);
}

int AssetManager::GetTexture(const std::string& assetId) {
    if (textures.find(assetId) == textures.end()) {
        Log::Err("texture: %s couldn't found in the registry", assetId.c_str());
        return -1;
    }

    return textures.at(assetId);
}

std::string AssetManager::GetTexture(int textureId) {
    for (auto it = textures.begin(); it != textures.end(); ++it)
        if (it->second == textureId)
            return it->first;

    return std::string();
}


TextureInf AssetManager::GetTextureInf(const std::string& assetId) {
    //int textureId = textures.at(assetId);
    //CommancheTextureInfo infR = crender->GetTextureInfo(textureId);

    //TextureInf inf;
    //inf.height = infR.height;
    //inf.width = infR.width;

    //return inf;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
unsigned int AssetManager::LoadCubeMap(std::vector<std::string> faces)
{
  stbi_set_flip_vertically_on_load(false);
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data =
        stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                   0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cout << "Cubemap texture failed to load at path: " << faces[i]
                << std::endl;
      stbi_image_free(data);
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
}

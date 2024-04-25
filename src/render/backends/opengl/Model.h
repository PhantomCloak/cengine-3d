#pragma once
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "render/backends/opengl/Shader.h"
#include "render/backends/opengl/Mesh.h"
#include "scene/node.h"

class Model : public Node
{
    public:
        Model(const char *path)
        {
						Name = path;
            loadModel(path);
        }
        void Draw(Shader &shader);	
        // model data
        std::vector<Ref<Mesh>> meshes;
        std::string directory;
				std::vector<Texture> textures_loaded;

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Ref<Mesh> processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
                                             std::string typeName);
};



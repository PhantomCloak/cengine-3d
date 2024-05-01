#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "log/log.h"
#include "io/filesystem.h"

unsigned int loadTexture(char const* path, bool hint = false) {
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;

				if(false)
				{
					if (nrComponents == 1) {
        	    format = GL_RED;
        	} else if (nrComponents == 3) {
        	    format = GL_SRGB;
        	} else if (nrComponents == 4) {
        	    format = GL_SRGB_ALPHA;
        	}
				} 
				else
				{
					if (nrComponents == 1) {
        	    format = GL_RED;
        	} else if (nrComponents == 3) {
        	    format = GL_RGB;
        	} else if (nrComponents == 4) {
        	    format = GL_RGBA;
        	}
				}


        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void Model::Draw(Shader& shader) {
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i]->Draw(shader);
}

void Model::loadModel(std::string path) {

		strPath = path;

    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}


bool f = false;
aiMatrix4x4 foo;

void Model::processNode(aiNode* node, const aiScene* scene) {
    // process all the node's meshes (if any)
   
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];


        Ref<Mesh> modelMesh = processMesh(mesh, scene);
				modelMesh->Name = mesh->mName.C_Str();
        meshes.push_back(modelMesh);
        AddChild(modelMesh);
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Ref<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
                          // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

				if(mesh->HasTangentsAndBitangents())
				{
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.BitTangent = vector;
				}


        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        } else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    // 2. specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_DISPLACEMENT, "texture_height");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		//if(normalMaps.size() > 0)
		//{
		//	Log::Inf("Yes");
		//	exit(0);
		//}

    // return a mesh object created from the extracted mesh data

    Ref<Mesh> result = CreateRef<Mesh>(vertices, indices, textures);

		float shyMan = 0;
		if(AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shyMan)) {
			Log::Err("Epic Fail");
		}
		else
		{
			result->shy = shyMan;
		}

		return result;
}

void Model::processMaterial(aiMesh* mesh) {

}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) { // if texture hasn't been loaded already, load it
            Texture texture;

						std::string fullPath = FileSys::GetParentDirectory(strPath) + "/" + str.C_Str();
						if(typeName == "texture_diffuse")
							texture.id = loadTexture(fullPath.c_str(), true);
						else
							texture.id = loadTexture(fullPath.c_str());

            std::cout << "Loading Texture: " << str.C_Str() << std::endl;
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // add to loaded textures
        }
    }
    return textures;
}

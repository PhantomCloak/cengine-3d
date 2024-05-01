#include "Mesh.h"
#include "log/log.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) : Node()
{
    this->vertices = vertices; 
		this->indices = indices;
    this->textures = textures;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // vertex tangent coords
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, Tangent));

		// vertex bit tangent coords
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)offsetof(Vertex, BitTangent));


    glBindVertexArray(0);
}


void Mesh::Draw(Shader &shader) 
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;

		shader.setMat4("model", Parent->GetModelMatrix() * this->GetModelMatrix());

		if(shy != 0)
		{
			shader.setFloat("material.shininess", shy);
		}

    for(unsigned int i = 0; i < textures.size(); i++)
    {
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);
        else if(name == "texture_height")
            number = std::to_string(normalNr++);

        glActiveTexture(GL_TEXTURE1 + i); // activate proper texture unit before binding

				//std::string debug(("material." + name + number).c_str());
				//std::cout << "dbg: " << debug << std::endl;
        shader.setInt(("material." + name + number).c_str(), i + 1);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    //glActiveTexture(GL_TEXTURE1);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
}



#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> verticies, std::vector<unsigned int> indicies, std::vector<Texture2D> textures) {

	this->verticies = verticies;
	this->indicies = indicies;
	this->textures = textures;

	setupMesh();
}

void Mesh::setupMesh() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(Vertex), &verticies[0], GL_STATIC_DRAW);
		
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), &indicies[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader* shader) {

	for (unsigned int i = 0; i < textures.size(); i++) {

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].texture);
		shader->setUniform("material.texture" + std::to_string(i), (int)i);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indicies.size()), GL_UNSIGNED_INT, 0);
	
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::Draw() {

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indicies.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
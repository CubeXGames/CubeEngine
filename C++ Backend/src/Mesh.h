#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Texture.h"
#include "Shader.h"
#include <vector>
#include <string>
using namespace glm;

struct Texture2D {

	unsigned int texture = 0;
	std::string path;
};

struct Vertex {

	vec3 pos;
	vec3 normal;
	vec2 texCoords;
};

class Mesh {

public:

	std::vector<Vertex> verticies;
	std::vector<unsigned int> indicies;
	std::vector<Texture2D> textures;
	unsigned int VAO, VBO, EBO;

	Mesh(std::vector<Vertex> verticies, std::vector<unsigned int> indicies, std::vector<Texture2D> textures);
	void Draw(Shader* shader);
	void Draw(); //for shadow mapping

private:

	void setupMesh();
};

#endif
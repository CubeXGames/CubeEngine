#ifndef MODEL_H
#define MODEL_H

#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>
#include <iostream>
using namespace Assimp;

constexpr std::string fileName(std::string& path) {

	std::string fileName = path;
	size_t last = fileName.find_last_of("\\/");
	if (last != std::string::npos) fileName = fileName.substr(last + 1);
	return fileName;
}

class Model {

public:

	static std::vector<Texture2D> loadedTextures;
	static Importer importer;

	std::vector<Mesh> meshes;
	const uint64_t id;
	bool failedLoading;

	Model();
	Model(std::string file, const uint64_t id);
	void Draw(Shader* shader);
	void Draw();
	
private:

	std::vector<Texture2D> texturesLoaded;
	std::string directory;

	void loadModel(const char* path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture2D> loadMaterialTextures(aiMaterial* mat, aiTextureType type);
};

#endif
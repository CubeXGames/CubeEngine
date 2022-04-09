#include "Model.h"
#include "Statics.h"

Importer Model::importer = Importer();

Model::Model() : id(0UL), failedLoading(false) {}

Model::Model(std::string file, const uint64_t id) : id(id) {

	loadModel(file.c_str());
}

void Model::Draw(Shader* shader) {

	for (auto& mesh : meshes) mesh.Draw(shader);
}

void Model::Draw() {

	for (auto& mesh : meshes) mesh.Draw();
}

void Model::loadModel(const char* file) {

	const aiScene* scene = Model::importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		
		Console::stream << "Error importing asset \"" << file << "\": " << importer.GetErrorString() << std::endl;
		failedLoading = false;
		return;
	}

	failedLoading = true;
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {

	std::vector<Vertex> vertexData;
	std::vector<unsigned int> indices;
	std::vector<Texture2D> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		
		Vertex vertex;

		vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.pos = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		if (mesh->mTextureCoords[0]) {
			
			vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		} else vertex.texCoords = vec2(0.0f, 0.0f);

		vertexData.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0) {
		
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture2D> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture2D> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture2D> emissionMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE);
		textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());
	}

	return Mesh(vertexData, indices, textures);
}

std::vector<Texture2D> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type) {
	
	std::vector<Texture2D> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		for (unsigned int j = 0; j < texturesLoaded.size(); j++) {	
			
			if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {
				
				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {

			Texture2D texture;
			std::string textureString = std::string(str.C_Str());
			texture.texture = Texture(fileName(textureString).c_str());
			texture.path = str.C_Str();
			textures.push_back(texture);
			texturesLoaded.push_back(texture);
		}
	}

	return textures;
}
#pragma once
#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "MultiFileReader.h"

constexpr size_t skyboxFaces = 6;

constexpr std::string getFileName(std::string& path) {

	std::string fileName = path;
	size_t last = fileName.find_last_of("\\/");
	if (last != std::string::npos) fileName = fileName.substr(last + 1);
	return fileName;
}

constexpr std::string getFileName(const char* path) {

	std::string fileName = path;
	size_t last = fileName.find_last_of("\\/");
	if (last != std::string::npos) fileName = fileName.substr(last + 1);
	return fileName;
}

enum class TextureType {

	FailedToLoad = 0,
	Texture2D = 1,
	Cubemap = 2
};

class Texture {

public:

	static bool textureFlipped;

	unsigned int texture = 0;
	TextureType type;

	Texture();
	Texture(const char* textureName);
	Texture(std::vector<std::string> faces);
	static void setFlipImageVertically(bool flip);

	operator unsigned int() {
		
		return texture;
	}
};

#endif
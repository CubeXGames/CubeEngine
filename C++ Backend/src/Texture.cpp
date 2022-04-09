#include "Texture.h"
#include "stb_image.hstb_image.h"

bool Texture::textureFlipped;

Texture::Texture() {

	texture = 0;
	type = TextureType::FailedToLoad;
}

Texture::Texture(const char* textureName) {

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned int length;
	int width, height, channels;
	const unsigned char* file = (const unsigned char*)MultiFileReader::ReadFile(textureName, &length);
	if (!file) { Console::stream << "ERROR: Failed to load texture \"" << textureName << "!" << '\n'; type = TextureType::FailedToLoad; return; }
	unsigned char* data = stbi_load_from_memory(file, length, &width, &height, &channels, 0);

	if (data) {

		GLenum format;
		if (channels == 1) format = GL_RED;
		else if (channels == 3) format = GL_RGB;
		else if (channels == 4) format = GL_RGBA;
		else format = GL_RGB;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else Console::stream << "ERROR: Failed to load texture \"" << textureName << "!" << '\n';

	free(data);
	type = TextureType::Texture2D;
}

Texture::Texture(std::vector<std::string> faces) {

	setFlipImageVertically(!Texture::textureFlipped); //skyboxes load wrong with everything flipped, so everything has to be unflipped

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int width, height, channels;
	for (unsigned int i = 0; i < faces.size(); i++) {

		std::string fileName = getFileName(faces[i]);

		unsigned int len;
		const unsigned char* file = (const unsigned char*)MultiFileReader::ReadFile(fileName.c_str(), len);
		if (file == nullptr) { Console::stream << "ERROR: Failed to load skybox texture " << fileName << "!" << '\n'; type = TextureType::FailedToLoad; return; }
		unsigned char* data = stbi_load_from_memory(file, len, &width, &height, &channels, 0);

		if (data != nullptr) {

			GLenum format;
			if (channels == 1) format = GL_RED;
			else if (channels == 3) format = GL_RGB;
			else if (channels == 4) format = GL_RGBA;
			else format = GL_RGB;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		} else {

			free(data);
			type = TextureType::FailedToLoad;
			Console::stream << "ERROR: Failed to load skybox texture " << fileName << "!" << '\n';
			return;
		}

		free(data);
		type = TextureType::Cubemap;
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	setFlipImageVertically(!Texture::textureFlipped);
}

void Texture::setFlipImageVertically(bool flip) {
	
	Texture::textureFlipped = flip;
	stbi_set_flip_vertically_on_load(flip);
}
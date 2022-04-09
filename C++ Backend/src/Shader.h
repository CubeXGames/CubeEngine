#ifndef SHADER_H
#define SHADER_H
constexpr size_t infoLogLength = 1024ULL;

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "Statics.h"
#include "MultiFileReader.h"
#include "Console.h"
using namespace glm;

class Shader {

public:

	static Shader defaultShader;
	unsigned int ID;
	bool successfulCompilation;

	Shader(const char* vertexPath, const char* fragmentPath);
	Shader();

	void use();
	static Shader& getDefaultShader();
	void setUniform(const std::string& name, bool value) const;
	void setUniform(const std::string& name, int value) const;
	void setUniform(const std::string& name, unsigned int value) const;
	void setUniform(const std::string& name, float value) const;
	void setUniformMat4(const std::string& name, mat4 value) const;
	void setUniformMat3(const std::string& name, mat3 value) const;
	void setUniform(const std::string& name, vec3 value) const;
	void setUniform(const char* name, bool value) const;
	void setUniform(const char* name, int value) const;
	void setUniform(const char* name, unsigned int value) const;
	void setUniform(const char* name, float value) const;
	void setUniformMat4(const char* name, mat4 value) const;
	void setUniformMat3(const char* name, mat3 value) const;
	void setUniform(const char*, vec3 value) const;
};

#endif
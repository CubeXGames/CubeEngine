#include "Shader.h"

Shader Shader::defaultShader;

Shader& Shader::getDefaultShader() {
	
	return Shader::defaultShader;
}

Shader::Shader() {

	ID = 0;
	successfulCompilation = false;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {

	ID = 0; //initialize the ID variable to avoid it being uninitialized if compilation fails

	const char* vShaderCode = MultiFileReader::ReadFile(vertexPath);
	const char* fShaderCode = MultiFileReader::ReadFile(fragmentPath);

	if (!vShaderCode) {
		
		successfulCompilation = false;
		Console::stream << "Vertex shader file " << vertexPath << " was not found!" << '\n';
		return;
	}

	if (!fShaderCode) {

		successfulCompilation = false;
		Console::stream << "Fragment shader file " << fragmentPath << " was not found!" << '\n';
		return;
	}

	unsigned int vertex, fragment;
	int success;
	char infoLog[infoLogLength];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {

		glGetShaderInfoLog(vertex, infoLogLength, nullptr, infoLog);
		Console::stream << "ERROR: Vertex shader compilation failed!\n" << infoLog << '\n';
		this->successfulCompilation = success;

		return;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {

		glGetShaderInfoLog(fragment, infoLogLength, nullptr, infoLog);
		Console::stream << "ERROR: Fragment shader compilation failed!\n" << infoLog << '\n';
		this->successfulCompilation = success;

		return;
	}

	ID = glCreateProgram();

	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {

		glGetProgramInfoLog(ID, infoLogLength, nullptr, infoLog);
		Console::stream << "ERROR: Shader program linking failed!\n" << infoLog << '\n';
		this->successfulCompilation = success;

		return;
	}

	this->successfulCompilation = success;
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() {

	if (!successfulCompilation) {
		
		Console::stream << "Failed to use shader because its compilation failed!" << '\n';
		return;
	}

	Statics::currentShader = ID;
	glUseProgram(ID);
}

void Shader::setUniform(const std::string& name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
void Shader::setUniform(const std::string& name, int value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
void Shader::setUniform(const std::string& name, unsigned int value) const { glUniform1ui(glGetUniformLocation(ID, name.c_str()), value); }
void Shader::setUniform(const std::string& name, float value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), value); }
void Shader::setUniformMat4(const std::string& name, mat4 value) const { glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value_ptr(value)); }
void Shader::setUniformMat3(const std::string& name, mat3 value) const { glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value_ptr(value)); }
void Shader::setUniform(const std::string& name, vec3 value) const { glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
void Shader::setUniform(const char* name, bool value) const { glUniform1i(glGetUniformLocation(ID, name), value); }
void Shader::setUniform(const char* name, int value) const { glUniform1i(glGetUniformLocation(ID, name), value); }
void Shader::setUniform(const char* name, unsigned int value) const { glUniform1ui(glGetUniformLocation(ID, name), value); }
void Shader::setUniform(const char* name, float value) const { glUniform1f(glGetUniformLocation(ID, name), value); }
void Shader::setUniformMat4(const char* name, mat4 value) const { glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, value_ptr(value)); }
void Shader::setUniformMat3(const char* name, mat3 value) const { glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, value_ptr(value)); }
void Shader::setUniform(const char* name, vec3 value) const { glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]); }
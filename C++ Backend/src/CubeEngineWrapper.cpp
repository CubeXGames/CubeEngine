#include "CubeEngineWrapper.h"

bool Initialize() {

	quit = false;
	float profilerTime = static_cast<float>(glfwGetTime());
	float elapsedTime = 0.0f;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	glfwWindowHint(GLFW_DEPTH_BITS, 32); //we want a 32 bit depth buffer

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //do fullscreen
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	if (!Console::InitializeConsole()) {
		
		//we can't print anything to the console, I'll leave as to why as an exercise for the reader
		ReleaseAllResourcesAndQuit();
		return false;
	}

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(1280, 720, Statics::getGameName(), nullptr, nullptr);
	if (window == nullptr) {

		Console::WriteLine("Failed to create GLFW window!");
		ReleaseAllResourcesAndQuit();
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwGetWindowSize(window, &Renderer::windowSizeX, &Renderer::windowSizeY);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {

		Console::WriteLine("Failed to initialize GLAD!");
		ReleaseAllResourcesAndQuit();
		return false;
	}

	elapsedTime = static_cast<float>(glfwGetTime()) - profilerTime;
	Console::stream << "Successfully initialized OpenGL in " << elapsedTime << " seconds." << '\n';
	profilerTime = static_cast<float>(glfwGetTime());

	if (!MultiFileReader::InitializeMultiFile(std::filesystem::path("./assets.mf"))) {

		Console::WriteLine("Failed to initialize multifile!");
		Quit();
		return false;
	}

	elapsedTime = static_cast<float>(glfwGetTime()) - profilerTime;
	Console::stream << "Successfully initialized multifile in " << elapsedTime << " seconds." << '\n';
	profilerTime = static_cast<float>(glfwGetTime());

	if (!Renderer::Initialize(window)) {

		Console::WriteLine("Failed to initialize renderer!");
		Quit();
		return false;
	}

	elapsedTime = static_cast<float>(glfwGetTime()) - profilerTime;
	Console::stream << "Successfully initialized renderer in " << elapsedTime << " seconds." << '\n';
	profilerTime = static_cast<float>(glfwGetTime());

	return true;
}

bool LoadModel(char* name, uint64_t uuid) {

	if (!quit) return Renderer::LoadModel(name, uuid)->failedLoading;
	else return false;
}

unsigned int LoadShader(char* vertexShader, char* fragmentShader) {

	if (!quit) return Renderer::LoadShader(vertexShader, fragmentShader)->ID;
	else return false;
}

bool InstantiateObject(uint64_t model, uint64_t material, unsigned int shader,
	float pos[3], float scale[3], float rot[4], bool transparent, uint64_t uuid) {

	if (!quit && !Object::objectMap.contains(uuid)) {

		vec3 posVec = vec3(pos[0], pos[1], pos[2]);
		vec3 scaleVec = vec3(scale[0], scale[1], scale[2]);
		quat rotVec = quat(scale[3], scale[0], scale[1], scale[2]);

		Object* obj = new Object(Renderer::models[model], Renderer::shaders[shader], Renderer::materials[material], posVec, scaleVec, rotVec, uuid);
		obj->transparencyFlag = transparent;

		Renderer::AddObject(obj);
		return true;
	} else return false;
}

bool SetObjectBaseData(uint64_t object, float pos[3], float scale[3], float rot[4]) {

	if (!quit) {

		if (Object::objectMap.contains(object)) {

			Object* obj = Object::objectMap[object];

			obj->pos.x = pos[0];
			obj->pos.y = pos[1];
			obj->pos.z = pos[2];
			obj->scale.x = scale[0];
			obj->scale.y = scale[1];
			obj->scale.z = scale[2];
			obj->rot.x = rot[0];
			obj->rot.y = rot[1];
			obj->rot.z = rot[2];
			obj->rot.w = rot[3];

			return true;
		}
	}

	return false;
}

bool SetObjectShader(uint64_t object, unsigned int shader) {

	if (!quit) {

		if (Object::objectMap.contains(object)) {

			Object* obj = Object::objectMap[object];
			obj->setShader(Renderer::shaders[shader]);

			return true;
		}
	}

	return false;
}

bool SetObjectModel(uint64_t object, uint64_t model) {

	if (!quit) {

		if (Object::objectMap.contains(object)) {

			Object* obj = Object::objectMap[object];
			obj->setModel(Renderer::models[model]);

			return true;
		}
	}

	return false;
}

bool SetObjectMaterial(uint64_t object, uint64_t material, bool transparent) {
	
	if (!quit) {

		if (Object::objectMap.contains(object)) {

			Object* obj = Object::objectMap[object];
			obj->setMaterial(Renderer::materials[material]);
			obj->transparencyFlag = transparent;

			return true;
		}
	}

	return false;
}

bool RemoveObject(uint64_t object) {

	if (!quit) {

		if(Object::objectMap.contains(object)) {

			Renderer::RemoveObject(Object::objectMap[object]);
			return true;
		}
	}
	
	return false;
}

bool SetDirectionalLightBaseData(float pos[3], float direction[3], bool active) {

	if (!quit && Renderer::dirLight != nullptr) {

		Renderer::dirLight->pos.x = pos[0];
		Renderer::dirLight->pos.y = pos[1];
		Renderer::dirLight->pos.z = pos[2];

		Renderer::dirLight->direction.x = direction[0];
		Renderer::dirLight->direction.y = direction[1];
		Renderer::dirLight->direction.z = direction[2];

		Renderer::dirLight->active = active;

		return true;
	} else return false;
}

bool SetDirectionalLightColorData(float ambient[3], float diffuse[3], float specular[3]) {

	if (!quit && Renderer::dirLight != nullptr) {

		Renderer::dirLight->ambient.r = ambient[0];
		Renderer::dirLight->ambient.g = ambient[1];
		Renderer::dirLight->ambient.b = ambient[2];

		Renderer::dirLight->diffuse.r = diffuse[0];
		Renderer::dirLight->diffuse.g = diffuse[1];
		Renderer::dirLight->diffuse.b = diffuse[2];

		Renderer::dirLight->specular.r = specular[0];
		Renderer::dirLight->specular.g = specular[1];
		Renderer::dirLight->specular.b = specular[2];

		return true;
	} else return false;
}

bool AddPointLight(float ambient[3], float diffuse[3], float specular[3], float strength,
	float constant, float linear, float quadratic, float pos[3], bool active, uint64_t uuid) {

	if (!quit) {

		try {

			vec3 ambientVec(ambient[0], ambient[1], ambient[2]);
			vec3 diffuseVec(diffuse[0], diffuse[1], diffuse[2]);
			vec3 specularVec(specular[0], specular[1], specular[2]);
			vec3 posVec(pos[0], pos[1], pos[2]);
			Light* pointLight = new Light(ambientVec, diffuseVec, specularVec, strength, constant, linear, quadratic);
			pointLight->setUUID(uuid);
			Renderer::AddPointLight(uuid, pointLight);

			return true;
		} catch (std::exception e) { Console::WriteLine(e.what()); return false; }
	} else return false;
}

bool SetPointLightBaseData(float pos[3], float strength, bool active, uint64_t light) {

	if (!quit && Renderer::pointLights.contains(light)) {

		Light* lightPtr = Renderer::pointLights[light];

		lightPtr->pos.x = pos[0];
		lightPtr->pos.y = pos[1];
		lightPtr->pos.z = pos[2];

		lightPtr->strength = strength;
		lightPtr->active = active;

		return true;
	} else return false;
}

bool SetPointLightColorData(float ambient[3], float diffuse[3], float specular[3],
	float constant, float linear, float quadratic, uint64_t light) {

	if (!quit && Renderer::pointLights.contains(light)) {

		Light* lightPtr = Renderer::pointLights[light];

		lightPtr->ambient.r = ambient[0];
		lightPtr->ambient.g = ambient[1];
		lightPtr->ambient.b = ambient[2];

		lightPtr->diffuse.r = diffuse[0];
		lightPtr->diffuse.g = diffuse[1];
		lightPtr->diffuse.b = diffuse[2];

		lightPtr->specular.r = specular[0];
		lightPtr->specular.g = specular[1];
		lightPtr->specular.b = specular[2];

		lightPtr->constant = constant;
		lightPtr->linear = linear;
		lightPtr->quadratic = quadratic;

		return true;
	} else return false;
}

bool RemovePointLight(uint64_t light) {

	if (!quit && Renderer::pointLights.contains(light)) {

		Renderer::RemovePointLight(Renderer::pointLights[light]);
		return true;
	} else return false;
}

bool SetCameraBaseData(float pos[3], float rot[4], float fov) {

	if (!quit && Renderer::camera) {

		Renderer::camera->fov = fov;

		Renderer::camera->pos.x = pos[0];
		Renderer::camera->pos.y = pos[1];
		Renderer::camera->pos.z = pos[2];

		Renderer::camera->rotation.x = rot[0];
		Renderer::camera->rotation.y = rot[1];
		Renderer::camera->rotation.z = rot[2];
		Renderer::camera->rotation.w = rot[3];

		return true;
	} else return false;
}

bool SetCameraClip(float near, float far) {

	if (!quit && Renderer::camera) {

		Renderer::camera->nearClip = near;
		Renderer::camera->farClip = far;

		return true;
	} else return false;
}

unsigned int LoadSkybox(char* left, char* right, char* up, char* down, char* front, char* back) {

	if (!quit) {

		std::vector<std::string> faces = {
			
			right, left, up, down, front, back
		};

		Texture* tex = new Texture(faces);
		if (tex->type == TextureType::FailedToLoad) return 0;
		Renderer::textures.insert(std::pair<unsigned int, Texture*>(tex->texture, tex));

		return tex->texture;
	} else return 0;
}

bool SetSkybox(unsigned int skybox) {

	if (!quit && Renderer::textures.contains(skybox)) {

		if (Renderer::textures[skybox]->type != TextureType::Cubemap) return false;

		Renderer::skybox = Renderer::textures[skybox];
		return true;
	} else return false;
}

bool LoadMaterial(float baseColor[3], float emissionStrength, float shininess, uint64_t uuid) {
	
	if (!quit && Renderer::materials.contains(uuid)) {

		vec3 baseColorVec(baseColor[0], baseColor[1], baseColor[2]);
		Material* mat = new Material(baseColorVec, shininess, emissionStrength);
		Renderer::materials.insert(std::pair<uint64_t, Material*>(uuid, mat));

		return true;
	} else return false;
}

bool RenderFrame() {

	if (!quit) {

		if (!(glfwWindowShouldClose(Renderer::windowHandle) || Renderer::markedForQuitting)) {

			frames++;

			if (!mouseCallbackCalled) mouseMovement = vec2(0, 0);
			mouseCallbackCalled = false;

			Renderer::DrawFrame();

			Console::FlushConsole();
			glfwSwapBuffers(Renderer::windowHandle);
			glfwPollEvents();
			return true;
		} else {

			glfwSetWindowShouldClose(Renderer::windowHandle, GLFW_TRUE);
			ReleaseAllResourcesAndQuit();
			return false;
		}
	} else return false;
}

void UseShader(unsigned int shader) {

	glUseProgram(shader);
	Statics::currentShader = shader;
}

const char* ReadFile(char* fileName, unsigned int* outLength) {

	return MultiFileReader::ReadFile(fileName, outLength);
}

bool Exists(char* fileName) {

	return MultiFileReader::Exists(fileName);
}

uint64_t GetFrameCount() {

	return frames;
}

void WriteLine(char* string) {

	Console::WriteLine(string);
}

void Quit() {
	
	Renderer::markedForQuitting = true;
}

void ReleaseAllResourcesAndQuit() {

	if (quit) return;

	for (auto& [k, v] : Renderer::models) {

		for (auto& mesh : v->meshes) {

			glDeleteVertexArrays(1, &mesh.VAO);
			glDeleteBuffers(1, &mesh.VBO);
			glDeleteBuffers(1, &mesh.EBO);
		}

		delete& v;
	}
	
	for (auto& [k, v] : Renderer::shaders) { glDeleteProgram(v->ID); if (v->ID != Shader::defaultShader.ID) delete v; }
	for (auto& [k, v] : Renderer::textures) { glDeleteTextures(1, &v->texture); delete v; }
	for (auto& [k, v] : Renderer::pointLights) { delete v; }
	for (auto& [k, v] : Renderer::materials) { delete v; }
	for (auto& [k, v] : Object::objectMap) { delete v; }

	Renderer::models.clear();
	Renderer::pointLights.clear();
	Renderer::materials.clear();
	Renderer::objects.clear();
	Object::objectMap.clear();

	if(Renderer::dirLight) delete Renderer::dirLight;
	if(Renderer::camera) delete Renderer::camera;

	Renderer::dirLight = nullptr;
	Renderer::camera = nullptr;

	glDeleteProgram(Renderer::skyboxShader.ID);
	glDeleteProgram(Renderer::postProcessShader.ID);

	glDeleteRenderbuffers(1, &Renderer::mainRBO);
	glDeleteFramebuffers(1, &Renderer::mainFBO);
	glDeleteTextures(1, &Renderer::mainTextureColorBuffer);
	glDeleteBuffers(1, &Renderer::matriciesUBO);
	glDeleteBuffers(1, &Renderer::lightingUBO);
	glDeleteBuffers(1, &Renderer::pointLightSSBO);
	glDeleteBuffers(1, &Renderer::exposureSSBO);

	glDeleteVertexArrays(1, &Renderer::ppqVAO);
	glDeleteVertexArrays(1, &Renderer::skyboxVAO);
	glDeleteBuffers(1, &Renderer::ppqVBO);
	glDeleteBuffers(1, &Renderer::skyboxVBO);

	Renderer::skyboxShader.ID = 0;
	Renderer::postProcessShader.ID = 0;
	Renderer::postProcessShader.successfulCompilation = false;
	Renderer::skyboxShader.successfulCompilation = false;

	Renderer::windowHandle = nullptr;
	Renderer::windowSizeX = 0;
	Renderer::windowSizeY = 0;
	Renderer::totalWindowPixels = 0;
	Renderer::backgroundColor = vec3(0, 0, 0);

	MultiFileReader::FreeMultifileBytes();

	int files = _fcloseall();
	if (files > 0) Console::stream << "WARNING: " << files << " files were not closed before closing the application! Although this usually doesn't cause any noticeable issues, "
		"it is a memory leak and should be fixed." << '\n';
	
	Console::FlushAndCloseConsole();
	glfwTerminate();
	quit = true;
	Renderer::markedForQuitting = false;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {

	Renderer::SetWindowSize(width, height);
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos) {

	mouseCallbackCalled = true;

	if (firstMouse) {

		lastX = static_cast<float>(xPos);
		lastY = static_cast<float>(yPos);
		firstMouse = false;
	}

	float deltaX = static_cast<float>(xPos) - lastX;
	float deltaY = lastY - static_cast<float>(yPos);
	lastX = static_cast<float>(xPos);
	lastY = static_cast<float>(yPos);

	const float sensitivity = 0.3f;
	mouseMovement = vec2(deltaX * sensitivity, deltaY * sensitivity);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {

	Renderer::camera->fov -= (float)yOffset;
	if (Renderer::camera->fov < 15.0f) Renderer::camera->fov = 15.0f;
	if (Renderer::camera->fov > 90.0f) Renderer::camera->fov = 90.0f;
}

void SetShaderBool(char* name, bool value) {

	glUniform1i(glGetUniformLocation(Statics::currentShader, name), value);
}

void SetShaderInt(char* name, int value) {

	glUniform1i(glGetUniformLocation(Statics::currentShader, name), value);
}

void SetShaderFloat(char* name, float value) {

	glUniform1f(glGetUniformLocation(Statics::currentShader, name), value);
}

void SetShaderVec2(char* name, float value[2]) {

	glUniform2fv(glGetUniformLocation(Statics::currentShader, name), 1, &value[0]);
}

void SetShaderVec3(char* name, float value[3]) {

	glUniform3fv(glGetUniformLocation(Statics::currentShader, name), 1, &value[0]);
}

void SetShaderVec4(char* name, float value[4]) {

	glUniform4fv(glGetUniformLocation(Statics::currentShader, name), 1, &value[0]);
}

void SetShaderMat2(char* name, float value[2][2], bool transpose) {

	glUniformMatrix2fv(glGetUniformLocation(Statics::currentShader, name), 1, transpose, &value[0][0]);
}

void SetShaderMat3(char* name, float value[3][3], bool transpose) {

	glUniformMatrix3fv(glGetUniformLocation(Statics::currentShader, name), 1, transpose, &value[0][0]);
}

void SetShaderMat4(char* name, float value[4][4], bool transpose) {

	glUniformMatrix4fv(glGetUniformLocation(Statics::currentShader, name), 1, transpose, &value[0][0]);
}
#pragma once
#include <iostream>
#include "Object.h"
#include "Statics.h"
#include "Camera.h"
#include "Light.h"
#include <algorithm>
#include <list>
#include <iterator>
#include "Console.h"

constexpr float postProcessQuad[] = {

   //vertex pos    texture coords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

constexpr float skyboxVertices[] = {

	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

constexpr float lengthSqr(vec3 vec) { return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z; }
constexpr GLbitfield clearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

class Renderer {

public:

	static std::unordered_map<unsigned int, Shader*> shaders;
	static std::unordered_map<unsigned int, Texture*> textures;
	static Shader postProcessShader, skyboxShader;
	static std::unordered_map<uint64_t, Model*> models;
	static std::list<Object*> objects;
	static std::unordered_map<uint64_t, Material*> materials;
	static Camera* camera;
	static Texture* skybox;
	static Light* dirLight;
	static std::unordered_map<uint64_t, Light*> pointLights;
	static float exposure, exposureMultiplier;
	static bool autoExposure;
	static bool markedForQuitting;

	static unsigned int mainFBO, mainRBO;
	static unsigned int mainTextureColorBuffer;
	static unsigned int matriciesUBO, lightingUBO;
	static unsigned int pointLightSSBO, exposureSSBO;
	static unsigned int ppqVAO, ppqVBO, skyboxVAO, skyboxVBO;
	static int windowSizeX, windowSizeY;
	static unsigned int totalWindowPixels;
	static GLFWwindow* windowHandle;

	static vec3 lightPos;
	static vec3 backgroundColor;

	static Model* LoadModel(std::string name, uint64_t id);
	static Shader* LoadShader(const char* vertex, const char* fragment);
	//Initializes the state of the renderer - default things, the main framebuffer, buffer objects, etc.
	static bool Initialize(GLFWwindow* window);
	static void SetWindowSize(int width, int height);
	static void AddObject(Object* object);
	static void RemoveObject(Object* id);
	static void AddPointLight(uint64_t uuid, Light* light);
	static void RemovePointLight(Light* light);
	static void UpdatePointLightSSBO();
	static void DrawFrame();

private:

	static bool firstFrame;

	static bool compareObjects(Object* a, Object* b);

	//Sets the parameters for the "material" variable in the shader "shader"
	static void SetMaterialParameters(Material* mat, Shader* shader);
	static void UpdateLightingUBO();
};
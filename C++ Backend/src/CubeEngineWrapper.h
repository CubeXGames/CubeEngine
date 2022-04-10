#pragma once
#ifndef CBW_H
#define CBW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Renderer.h"
#include "Random.h"

#ifdef _WIN32
#define EXPORTED __declspec(dllexport)
#else
#define EXPORTED
#endif

long frames;
int windowWidth, windowHeight;
float cameraSpeed;
float lastX = 0, lastY = 0;
vec2 mouseMovement;
bool firstMouse = true;
bool mouseCallbackCalled;
quat verticalQuat;
bool quit = true;

extern "C" {

	EXPORTED bool __cdecl Initialize();
	EXPORTED bool __cdecl LoadModel(char* name, uint64_t uuid);
	EXPORTED unsigned int __cdecl LoadShader(char* vertexShader, char* fragmentShader);
	EXPORTED bool __cdecl InstantiateObject(uint64_t model, uint64_t material, unsigned int shader,
		float pos[3], float scale[3], float rot[4], bool transparent, uint64_t uuid);
	EXPORTED bool __cdecl SetObjectBaseData(uint64_t object, float pos[3], float scale[3], float rot[4]);
	EXPORTED bool __cdecl SetObjectShader(uint64_t object, unsigned int shader);
	EXPORTED bool __cdecl SetObjectModel(uint64_t object, uint64_t model);
	EXPORTED bool __cdecl SetObjectMaterial(uint64_t object, uint64_t material, bool transparent);
	EXPORTED bool __cdecl RemoveObject(uint64_t object);
	EXPORTED bool __cdecl SetDirectionalLightBaseData(float pos[3], float direction[3], bool active);
	EXPORTED bool __cdecl SetDirectionalLightColorData(float ambient[3], float diffuse[3], float specular[3]);
	EXPORTED bool __cdecl AddPointLight(float ambient[3], float diffuse[3], float specular[3], float strength,
		float constant, float linear, float quadratic, float pos[3], bool active, uint64_t uuid);
	EXPORTED bool __cdecl SetPointLightBaseData(float pos[3], float strength, bool active, uint64_t light);
	EXPORTED bool __cdecl SetPointLightColorData(float ambient[3], float diffuse[3], float specular[3],
		float constant, float linear, float quadratic, uint64_t light);
	EXPORTED bool __cdecl RemovePointLight(uint64_t light);
	EXPORTED bool __cdecl SetCameraBaseData(float pos[3], float rot[4], float fov);
	EXPORTED bool __cdecl SetCameraClip(float near, float far);
	EXPORTED unsigned int __cdecl LoadSkybox(char* left, char* right, char* up, char* down, char* front, char* back);
	EXPORTED bool __cdecl SetSkybox(unsigned int skybox);
	EXPORTED bool __cdecl LoadMaterial(float baseColor[3], float emissionStrength, float shininess, uint64_t uuid);
	EXPORTED bool __cdecl RenderFrame();
	EXPORTED void __cdecl UseShader(unsigned int shader);
	EXPORTED void __cdecl SetShaderBool(char* name, bool value);
	EXPORTED void __cdecl SetShaderInt(char* name, int value);
	EXPORTED void __cdecl SetShaderFloat(char* name, float value);
	EXPORTED void __cdecl SetShaderVec2(char* name, float value[2]);
	EXPORTED void __cdecl SetShaderVec3(char* name, float value[3]);
	EXPORTED void __cdecl SetShaderVec4(char* name, float value[4]);
	EXPORTED void __cdecl SetShaderMat2(char* name, float value[2][2], bool transpose);
	EXPORTED void __cdecl SetShaderMat3(char* name, float value[3][3], bool transpose);
	EXPORTED void __cdecl SetShaderMat4(char* name, float value[4][4], bool transpose);
	EXPORTED bool __cdecl ReadFile(char* fileName, unsigned int* outLength, char* outBytes);
	EXPORTED bool __cdecl Exists(char* fileName);
	EXPORTED uint64_t __cdecl GetFrameCount();
	EXPORTED void __cdecl WriteLine(char* string);
	EXPORTED void __cdecl Quit();
}

void ReleaseAllResourcesAndQuit();
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

#endif

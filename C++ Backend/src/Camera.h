#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace glm;

class Camera {

public:

	vec3 pos;
	quat rotation;
	float fov, nearClip, farClip;

	Camera();
	Camera(vec3 pos);
	Camera(vec3 pos, float near, float far);
	mat4 getViewMatrix();
	void setRotation(vec3 eulerAngles);
	void rotate(vec3 eulerAngles);
	vec3 getForward();
	vec3 getRight();
	//void updateCameraVectors();
};
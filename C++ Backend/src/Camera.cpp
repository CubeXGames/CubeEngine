#include "Camera.h"
#define USE_MATH_DEFINES
#include <cmath>

Camera::Camera() {

	pos = vec3(0.0f, 0.0f, 0.0f);
	rotation = quat(1, 0, 0, 0);
	fov = 90.0f;
	nearClip = 0.05f;
	farClip = 1000.0f;
}

Camera::Camera(vec3 pos) : pos(pos) {

	rotation = quat(1, 0, 0, 0);
	fov = 90.0f;
	nearClip = 0.05f;
	farClip = 1000.0f;
}

Camera::Camera(vec3 pos, float near, float far) : pos(pos), nearClip(near), farClip(far) {

	rotation = quat(1, 0, 0, 0);
	fov = 90.0f;
}

mat4 Camera::getViewMatrix() {

	mat4 rot = mat4_cast(conjugate(rotation));
	mat4 translation = translate(mat4(1.0f), -pos);
	return rot * translation;
}

void Camera::setRotation(vec3 eulerAngles) {

	rotation = normalize(quat(vec3(radians(eulerAngles.x), radians(eulerAngles.y), radians(eulerAngles.z))));
}

void Camera::rotate(vec3 eulerAngles) {

	rotation = normalize(rotation * inverse(quat(vec3(radians(eulerAngles.x), radians(eulerAngles.y), radians(eulerAngles.z)))));
}

vec3 Camera::getForward() {

	quat forward = rotation * quat(0, 0, 0, -1) * conjugate(rotation);
	return vec3(forward.x, forward.y, forward.z);
}

vec3 Camera::getRight() {

	return normalize(cross(getForward(), vec3(0, 1, 0)));
}

/*void Camera::updateCameraVectors() {

	quat aroundY = angleAxis(radians(-xRot), vec3(0, 1, 0));
	quat aroundX = angleAxis(radians(yRot), vec3(1, 0, 0));
	quat aroundZ = angleAxis(radians(zRot), vec3(0, 0, 1));

	rotation = aroundY * aroundX * aroundZ;
}*/
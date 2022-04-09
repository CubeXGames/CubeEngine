#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Shader.h"

using namespace glm;
constexpr unsigned int shadowMapWidth = 2048U;
constexpr unsigned int shadowMapHeight = 2048U;

struct DirLight {

	bool activeLight;
	vec4 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float strength;
};

struct PointLight {

	vec4 pos;
	uint activeLight;
	uint padding[3] = { 0U, 0U, 0U };
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float constant;
	float linear;
	float quadratic;
	float strength;

	PointLight(vec3 pos, bool activeLight, vec3 ambient, vec3 diffuse, vec3 specular, float constant, float linear, float quadratic, float strength)
		: pos(pos, 1.0f), activeLight(activeLight), ambient(ambient, 1.0f), diffuse(diffuse, 1.0f), specular(specular, 1.0f), constant(constant),
		linear(linear), quadratic(quadratic), strength(strength) {}

	PointLight() {
		
		pos = vec4(0.0f);
		activeLight = false;
		ambient = vec4(0.0f);
		diffuse = vec4(0.0f);
		specular = vec4(0.0f);
		constant = 0;
		linear = 0;
		quadratic = 0;
		strength = 0;
	}
};

enum class LightType {

	Directional = 0,
	Point = 1,
	None = -1
};

class Light {

public:

	LightType type;
	bool active;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float strength;
	
	//Only for directional lights.
	vec3 direction;

	//Only for point lights.
	float constant, linear, quadratic;
	vec3 pos;

	Light();
	Light(LightType lightType);
	Light(vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction, float strength);
	Light(vec3 ambient, vec3 diffuse, vec3 specular, float strength, float constant, float linear, float quadratic);
	uint64_t getUUID() const;
	void setUUID(uint64_t);

	bool operator !=(Light other) {

		return id != other.getUUID();
	}

	bool operator ==(Light other) {

		return id == other.getUUID();
	}

	bool operator !=(const Light other) const {

		return id != other.getUUID();
	}

	bool operator ==(const Light other) const {

		return id == other.getUUID();
	}

private:

	uint64_t id;
};

#endif
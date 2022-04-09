#pragma once
#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include "Model.h"
#include "Shader.h"
using namespace glm;

struct Material {

	vec3 baseColor;
	float emissionStrength;
	float shininess;

	Material(vec3 baseColor, float shininess, float emissionStrength) : baseColor(baseColor),
		emissionStrength(emissionStrength), shininess(shininess) {}

	Material() : baseColor(0, 0, 0), emissionStrength(0), shininess(0) {}
};

class Object {

public:

	static std::unordered_map<uint64_t, Object*> objectMap;

	vec3 pos;
	vec3 scale;
	quat rot;
	Model* model;
	Shader* shader;
	Material* material;
	bool transparencyFlag;

	Object();
	Object(Model* model, Shader* shader, Material* material, uint64_t uuid);
	Object(Model* model, Shader* shader, Material* material, vec3& pos, vec3& scale, quat& rot, uint64_t uuid);

	mat4 getModelMatrix() const;
	void setRotation(vec3& eulerAngles);
	void rotate(vec3& eulerAngles);
	vec3 getForwardVector() const;
	uint64_t getUUID() const;
	void setUUID(uint64_t uuid);
	void setModel(Model* m);
	void setShader(Shader* s);
	void setMaterial(Material* m);

	bool operator !=(Object other) {

		return id != other.getUUID();
	}

	bool operator ==(Object other) {

		return id == other.getUUID();
	}

	bool operator !=(const Object other) const {

		return id != other.getUUID();
	}

	bool operator ==(const Object other) const {

		return id == other.getUUID();
	}

private:

	uint64_t id;
};

#endif
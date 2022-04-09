#include "Light.h"

Light::Light() : id(0ULL) {

	this->ambient = vec3(0, 0, 0);
	this->diffuse = vec3(0, 0, 0);
	this->specular = vec3(0, 0, 0);
	this->direction = vec3(0, 0, 0);
	this->strength = 0;
	this->type = LightType::None;
	this->active = false;

	constant = 0;
	linear = 0;
	quadratic = 0;
	pos = vec3(0, 0, 0);
}

Light::Light(LightType lightType) : id(0ULL) {

	this->ambient = vec3(0, 0, 0);
	this->diffuse = vec3(0, 0, 0);
	this->specular = vec3(0, 0, 0);
	this->direction = vec3(0, 0, 0);
	this->strength = 0;
	this->type = lightType;
	this->active = false;

	constant = 0;
	linear = 0;
	quadratic = 0;
	pos = vec3(0, 0, 0);
}

Light::Light(vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction, float strength) : id(0ULL) {

	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->direction = normalize(direction);
	this->strength = strength;
	this->type = LightType::Directional;
	this->active = true;

	constant = 0;
	linear = 0;
	quadratic = 0;
	pos = vec3(0, 0, 0);
}

Light::Light(vec3 ambient, vec3 diffuse, vec3 specular, float strength, float constant, float linear, float quadratic) : id(0ULL) {

	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->strength = strength;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
	this->type = LightType::Point;
	this->active = true;

	direction = vec3(0, 0, 0);
	pos = vec3(0, 0, 0);
}

uint64_t Light::getUUID() const { return id; }
void Light::setUUID(uint64_t uuid) { this->id = uuid; }
#include "Object.h"
std::unordered_map<uint64_t, Object*> Object::objectMap = std::unordered_map<uint64_t, Object*>();

Object::Object() : pos(0, 0, 0), scale(0, 0, 0), rot(0, 0, 0, 0), model(nullptr), shader(nullptr), material(nullptr), transparencyFlag(false), id(0) {}

Object::Object(Model* model, Shader* shader, Material* material, uint64_t uuid) : model(model), shader(shader), material(material), id(uuid) {

	transparencyFlag = false;
	
	pos = vec3(0.0f, 0.0f, 0.0f);
	scale = vec3(1.0f, 1.0f, 1.0f);
	rot = quat(1.0f, 0.0f, 0.0f, 0.0f);
}

Object::Object(Model* model, Shader* shader, Material* material, vec3& pos, vec3& scale, quat& rot, uint64_t uuid) : model(model), shader(shader), material(material), id(uuid) {

	transparencyFlag = false;

	this->pos = pos;
	this->scale = scale;
	this->rot = rot;
}

mat4 Object::getModelMatrix() const {

	return mat4_cast(conjugate(rot)) * glm::scale(mat4(1.0f), scale) * translate(mat4(1.0f), -pos);
}

void Object::setRotation(vec3& eulerAngles) {

	rot = normalize(quat(vec3(radians(eulerAngles.x), radians(eulerAngles.y), radians(eulerAngles.z))));
}

void Object::rotate(vec3& eulerAngles) {

	rot = normalize(rot * inverse(quat(vec3(radians(eulerAngles.x), radians(eulerAngles.y), radians(eulerAngles.z)))));
}

vec3 Object::getForwardVector() const {

	quat forward = rot * quat(0, 0, 0, -1) * conjugate(rot);
	return vec3(forward.x, forward.y, forward.z);
}

uint64_t Object::getUUID() const {
	
	return id;
}

void Object::setUUID(uint64_t uuid) {

	id = uuid;
}

void Object::setModel(Model* m) {

	this->model = m;
}

void Object::setShader(Shader* s) {
	
	this->shader = s;
}

void Object::setMaterial(Material* m) {
	
	this->material = m;
}
#include "Renderer.h"

std::unordered_map<unsigned int, Shader*> Renderer::shaders;
std::unordered_map<unsigned int, Texture*> Renderer::textures;
Shader Renderer::postProcessShader, Renderer::skyboxShader;
std::unordered_map<uint64_t, Model*> Renderer::models;
std::list<Object*> Renderer::objects;
std::unordered_map<uint64_t, Material*> Renderer::materials;
Light* Renderer::dirLight;
std::unordered_map<uint64_t, Light*> Renderer::pointLights;
vec3 Renderer::backgroundColor;
Camera* Renderer::camera;
Texture* Renderer::skybox;
float Renderer::exposure, Renderer::exposureMultiplier;
bool Renderer::autoExposure = false, Renderer::firstFrame;
bool Renderer::markedForQuitting = false;

unsigned int Renderer::mainFBO, Renderer::mainRBO;
unsigned int Renderer::mainTextureColorBuffer;
unsigned int Renderer::matriciesUBO, Renderer::lightingUBO;
unsigned int Renderer::pointLightSSBO, Renderer::exposureSSBO;
unsigned int Renderer::ppqVAO, Renderer::ppqVBO;
unsigned int Renderer::skyboxVAO, Renderer::skyboxVBO;
int Renderer::windowSizeX, Renderer::windowSizeY;
unsigned int Renderer::totalWindowPixels;
GLFWwindow* Renderer::windowHandle;

bool Renderer::Initialize(GLFWwindow* window) {

	Shader::defaultShader = Shader("singleColorVertex.glsl", "unlitFragmentShader.glsl"); //compile default shader
	if (!Shader::defaultShader.successfulCompilation) { Console::WriteLine("Compilation of the default shader failed!"); return false; }
	Shader::defaultShader.setUniform("color", vec3(1.0f, 0.0f, 1.0f)); //set its color to magenta to make it obvious the shader hasn't compiled (like Unity does)

	Renderer::postProcessShader = Shader("postProcessVert.glsl", "postProcessFrag.glsl"); //compile post processing shader
	if (!Renderer::postProcessShader.successfulCompilation) { Console::WriteLine("Compilation of the post process shader failed!"); return false; }

	Renderer::skyboxShader = Shader("skyboxVert.glsl", "skyboxFrag.glsl");
	if (!Renderer::skyboxShader.successfulCompilation) { Console::WriteLine("Compilation of the skybox shader failed!"); return false; }

	Renderer::backgroundColor = vec3(0.0f, 0.0f, 0.0f); //not really necessary, will be set anyway when the scene loads
	Renderer::exposure = 1; //set the default exposure as 1
	Renderer::exposureMultiplier = 1.025f;
	Renderer::firstFrame = true;
	Renderer::camera = new Camera(vec3(0, 0, 0));
	Renderer::dirLight = new Light(vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 0, 0), 0.0f);
	Renderer::dirLight->active = false;

	Renderer::windowHandle = window;
	glfwGetWindowSize(window, &Renderer::windowSizeX, &Renderer::windowSizeY);
	Renderer::totalWindowPixels = Renderer::windowSizeX * Renderer::windowSizeY;

	glEnable(GL_DEPTH_TEST); //set up all the parameters for OpenGL (depth testing function and backface culling)
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	
	glGenFramebuffers(1, &Renderer::mainFBO); //make the first rendering pass framebuffer (first rendered to, then given to post processing to generate the output frame)

	glGenTextures(1, &Renderer::mainTextureColorBuffer); //make the main color buffer
	glBindTexture(GL_TEXTURE_2D, Renderer::mainTextureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Renderer::windowSizeX, Renderer::windowSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //setting a bunch of texture parameters to make it look fine even if something weird happens
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenRenderbuffers(1, &Renderer::mainRBO); //make the main renderbuffer (for depth / stencil buffer data)
	glBindRenderbuffer(GL_RENDERBUFFER, Renderer::mainRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, Renderer::windowSizeX, Renderer::windowSizeY);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {

		Console::stream << "ERROR: Main framebuffer is not working! Error message: "
			<< glCheckFramebufferStatus(GL_FRAMEBUFFER) << '\n';
		return false;
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, Renderer::mainFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Renderer::mainTextureColorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Renderer::mainRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0); //clean up

	glGenVertexArrays(1, &Renderer::ppqVAO); //set up the post processing square
	glGenBuffers(1, &Renderer::ppqVBO);
	glBindVertexArray(Renderer::ppqVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Renderer::ppqVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(postProcessQuad), &postProcessQuad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //we need position inputs and texture coord inputs
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glGenVertexArrays(1, &Renderer::skyboxVAO); //set up the skybox
	glGenBuffers(1, &Renderer::skyboxVBO);
	glBindVertexArray(Renderer::skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Renderer::skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //we just need position coordinates
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0); //clean up some more

	glGenBuffers(1, &matriciesUBO); //set up uniform buffer object for matricies to reduce setting of uniforms
	glGenBuffers(1, &lightingUBO);  //do the same with lighting data

	glBindBuffer(GL_UNIFORM_BUFFER, matriciesUBO);
	glBufferData(GL_UNIFORM_BUFFER, 192, nullptr, GL_DYNAMIC_DRAW); //2 matrix 4x4s = 128
	glBindBuffer(GL_UNIFORM_BUFFER, lightingUBO);
	glBufferData(GL_UNIFORM_BUFFER, 144, nullptr, GL_DYNAMIC_DRAW); //two vec3's + one dirLight = 112

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, matriciesUBO, 0, 192);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, lightingUBO, 0, 144);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, value_ptr(Renderer::backgroundColor)); //fill all the point lights with zero values to avoid undefined behavior
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, value_ptr(vec3(0, 0, 0)));

	glBindBuffer(GL_UNIFORM_BUFFER, 0); //much clean, very sweep 🧹

	//set up the shader storage buffer objects
	glGenBuffers(1, &Renderer::pointLightSSBO); //for storing all of the point lights (because of dynamic size)
	glGenBuffers(1, &Renderer::exposureSSBO); //for calculating the average brightness of the scene (for auto exposure, because can be written to from the shader)

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Renderer::pointLightSSBO);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, Renderer::pointLightSSBO, 0, sizeof(PointLight));
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(PointLight), nullptr, GL_STATIC_DRAW);

	/*glBindBuffer(GL_SHADER_STORAGE_BUFFER, Renderer::exposureSSBO); //will get auto exposure working later
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, Renderer::exposureSSBO, 0, 12);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 12, nullptr, GL_STREAM_DRAW);
	Renderer::exposure = 1.0f;*/

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); //oh no! the cleanliness value has overflowed and is now -2147483648!

	return true; //Initializing the renderer has succeeded, so return true
}

void Renderer::DrawFrame() {
	
	mat4 projection = perspective(radians(camera->fov), (float)Renderer::windowSizeX / (float)Renderer::windowSizeY, camera->nearClip, camera->farClip);
	mat4 view = camera->getViewMatrix();
	Renderer::objects.sort(Renderer::compareObjects); //sort the objects (see the Renderer::compareObjects function for why)
	
	glBindBuffer(GL_UNIFORM_BUFFER, matriciesUBO); //update global matrix variables
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, value_ptr(view)); //set view matrix
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, value_ptr(projection)); //set projection matrix
	glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, value_ptr(mat4(mat3(view)))); //set skybox view matrix (basically view matrix but no position)

	Renderer::UpdateLightingUBO(); //update global lighting buffers
	Renderer::UpdatePointLightSSBO();

	glBindFramebuffer(GL_FRAMEBUFFER, mainFBO); //start the first rendering pass
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
	glClear(clearBits);
	glEnable(GL_DEPTH_TEST);

	if (skybox != nullptr && skybox->type == TextureType::Cubemap) { //if there is a skybox, render the skybox

		glDepthMask(GL_FALSE);
		Renderer::skyboxShader.use();

		glBindVertexArray(Renderer::skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, *Renderer::skybox);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
	}

	bool transparentStartLock = false;
	auto transparentStartIndex = Renderer::objects.begin();

	for (auto it = Renderer::objects.begin(); it != Renderer::objects.end(); it++) { //start rendering of non-transparent objects
		
		Object* obj = *it;
		if (obj->transparencyFlag) { //for finding where the transparent objects start in the list of objects

			if (!transparentStartLock) { //definitely could be merged into the sorting function, but would be around the same speed so doesn't really matter
				
				transparentStartLock = true;
				transparentStartIndex = it;
			}

			continue;
		}
		
		if (obj->shader->ID != Statics::currentShader) {

			obj->shader->use();
			Statics::currentShader = obj->shader->ID;
		}
		
		mat4 model = obj->getModelMatrix();
		obj->shader->setUniformMat4("model", model);
		obj->shader->setUniformMat3("tiModel", transpose(inverse(model))); //for normal calculations
		Renderer::SetMaterialParameters(obj->material, obj->shader);
		obj->model->Draw(obj->shader);
	}

	Statics::currentShader = 0;
	glDisable(GL_DEPTH_TEST); //disable depth test for drawing transparent objects (if any) and drawing post processing
	if (transparentStartLock) { //only render transparent objects if there are any

		for (auto it = transparentStartIndex; it != Renderer::objects.end(); it++) { //draw transparent objects

			Object* obj = *it;
			if (obj->shader->ID != Statics::currentShader) {

				obj->shader->use();
				Statics::currentShader = obj->shader->ID;
			}

			mat4 model = obj->getModelMatrix();
			obj->shader->setUniformMat4("model", model);
			obj->shader->setUniformMat3("tiModel", transpose(inverse(model))); //for normal calculations
			Renderer::SetMaterialParameters(obj->material, obj->shader);
			obj->model->Draw(obj->shader);
		}
	}

	Statics::currentShader = 0;
	
	/*if (autoExposure) {

		double totalBrightness;

		if (!firstFrame) {

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, exposureSSBO);
			glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 4, 8, &totalBrightness); //get the total brightness 
		} else totalBrightness = Renderer::totalWindowPixels;

		float lum = (float)(totalBrightness / Renderer::totalWindowPixels);
		exposure = lerp(exposure, 0.5f / lum * exposureMultiplier, 0.05f);
		exposure = glm::clamp(exposure, 0.1f, 3.5f); //set the minimum and maximum exposure to whatever you want
	}*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0); //post processing pass
	glClear(clearBits); //clear everything

	glBindFramebuffer(GL_READ_FRAMEBUFFER, Renderer::mainFBO); //copy the depth buffer to the post processing buffer so it can be used during post processing
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, Renderer::windowSizeX, Renderer::windowSizeY, 0, 0, Renderer::windowSizeX, Renderer::windowSizeY, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	glActiveTexture(GL_TEXTURE0); //set texture slot 0 as the previously rendered to image
	glBindTexture(GL_TEXTURE_2D, mainTextureColorBuffer);
	
	Renderer::postProcessShader.use();
	Renderer::postProcessShader.setUniform("exposure", Renderer::exposure);
	glBindVertexArray(ppqVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6); //render the post processing as two triangles covering the whole screen (kind of weird if you think about it, but whatever)

	Renderer::firstFrame = false;
}

void Renderer::SetMaterialParameters(Material* mat, Shader* shader) {
	
	shader->setUniform("material.baseColor", mat->baseColor);
	shader->setUniform("material.emissionStrength", mat->emissionStrength);
	shader->setUniform("material.shininess", mat->shininess);
}

void Renderer::UpdateLightingUBO() {
	
	glBindBuffer(GL_UNIFORM_BUFFER, lightingUBO); //set all the light variables
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, value_ptr(Renderer::backgroundColor));

	vec4 viewPos(camera->pos, 0.0f);
	unsigned int numPointLights = Renderer::pointLights.size();
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, value_ptr(viewPos));
	glBufferSubData(GL_UNIFORM_BUFFER, 32, 4, &numPointLights);

	if (dirLight != nullptr && dirLight->active) {

		vec4 direction(dirLight->direction, 0.0f);
		vec4 ambient(dirLight->ambient, 0.0f);
		vec4 diffuse(dirLight->diffuse, 0.0f);
		vec4 specular(dirLight->specular, 0.0f);
		uint active = true;
		float strength = dirLight->strength;

		glBufferSubData(GL_UNIFORM_BUFFER, 48, 4, &active);
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 16, value_ptr(direction));
		glBufferSubData(GL_UNIFORM_BUFFER, 80, 16, value_ptr(ambient));
		glBufferSubData(GL_UNIFORM_BUFFER, 96, 16, value_ptr(diffuse));
		glBufferSubData(GL_UNIFORM_BUFFER, 112, 16, value_ptr(specular));
		glBufferSubData(GL_UNIFORM_BUFFER, 128, 4, &strength);
	} else {
		
		vec4 direction(0.0f);
		vec4 ambient(0.0f);
		vec4 diffuse(0.0f);
		vec4 specular(0.0f);
		uint active = false;
		float strength = 0.0f;
		
		glBufferSubData(GL_UNIFORM_BUFFER, 48, 4, &active);
		glBufferSubData(GL_UNIFORM_BUFFER, 52, 16, value_ptr(direction));
		glBufferSubData(GL_UNIFORM_BUFFER, 68, 16, value_ptr(ambient));
		glBufferSubData(GL_UNIFORM_BUFFER, 84, 16, value_ptr(diffuse));
		glBufferSubData(GL_UNIFORM_BUFFER, 100, 16, value_ptr(specular));
		glBufferSubData(GL_UNIFORM_BUFFER, 116, 4, &strength);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0); //clean up
}

Model* Renderer::LoadModel(std::string name, uint64_t id) {

	Model* model = new Model(name, id);
	Renderer::models.insert(std::pair<uint64_t, Model*>(id, model));
	return model;
}

/*Model* Renderer::GetModel(std::string name, bool loadIfNonexistent) {

	if (Renderer::models.contains(name)) return &(Renderer::models[name]);
	else return loadIfNonexistent ? LoadModel(name) : nullptr;
}*/

Shader* Renderer::LoadShader(const char* vertex, const char* fragment) {

	Shader* s = new Shader(vertex, fragment);
	if (!s->successfulCompilation) {

		Console::WriteLine("Shader is invalid! Returning default shader.");
		return &Shader::defaultShader;
	} else {

		Renderer::shaders.insert(std::pair<unsigned int, Shader*>(s->ID, s));
		return s;
	}
}

void Renderer::AddObject(Object* object) {

	Renderer::objects.push_back(object);
	Object::objectMap.insert(std::pair<uint64_t, Object*>(object->getUUID(), object));
}

void Renderer::RemoveObject(Object* object) {

	Renderer::objects.remove(object);
	Object::objectMap.erase(object->getUUID());
	delete object;
}

void Renderer::AddPointLight(uint64_t uuid, Light* light) {

	Renderer::pointLights.insert(std::pair<uint64_t, Light*>(uuid, light));
	Renderer::UpdatePointLightSSBO();
}

void Renderer::RemovePointLight(Light* light) {

	Renderer::pointLights.erase(light->getUUID());
	delete light;
}

void Renderer::UpdatePointLightSSBO() {

	const unsigned int size = Renderer::pointLights.size();
	glBindBuffer(GL_UNIFORM_BUFFER, Renderer::lightingUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 32, 4, &size);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	PointLight* lights = new PointLight[size]; //make an array of PointLights with "size" elements
	unsigned int i = 0;
	for (auto& [id, l] : Renderer::pointLights) {

		if (i > size - 1) break; //if i is somehow bigger than the size of lights, break from the loop to avoid a buffer overrun

		PointLight pl(l->pos, l->active, l->ambient, l->diffuse, l->specular,
			l->constant, l->linear, l->quadratic, l->strength);

		lights[i] = pl;
		i++;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Renderer::pointLightSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(PointLight), &lights[0], GL_DYNAMIC_DRAW); //copy the array of lights to the graphics card
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); //clean up

	delete[] lights; //make sure to free the memory for lights (allocated on the heap instead of the stack, so not automatically deallocated)
}

void Renderer::SetWindowSize(int width, int height) {

	if (width == 0 || height == 0) return; //this isn't likely to happen, but just in case

	Renderer::windowSizeX = width;
	Renderer::windowSizeY = height;

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, Renderer::mainFBO);
	glBindTexture(GL_TEXTURE_2D, Renderer::mainTextureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindRenderbuffer(GL_RENDERBUFFER, Renderer::mainRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Renderer::mainRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Renderer::totalWindowPixels = width * height; //update the number of pixels in the exposure SSBO to make the average be right
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, exposureSSBO);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4, &Renderer::totalWindowPixels);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

bool Renderer::compareObjects(Object* a, Object* b) {

	if (b->transparencyFlag && !a->transparencyFlag) return true; //if b is transparent and a is not, b should behind a
	else if (a->transparencyFlag && b->transparencyFlag)
		return lengthSqr(Renderer::camera->pos - a->pos) >= lengthSqr(Renderer::camera->pos - b->pos); //if both are transparent, sort based on depth
	else return a->shader->ID > b->shader->ID; //if both objects are not transparent, sort based on shader to decrease
}                                            //the amount of calls to the GPU (e.g. setting the shader or uniform variables)
#include "Main.h"
#include "Shader.h"
#include "Texture.h"
#include <iostream>
#include <ctime>
#include <thread>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Random.h"
#include "Camera.h"
#include "Object.h"
#include "Model.h"
#include "MultiFileReader.h"
#include "Renderer.h"
#pragma warning(disable: 4244)

using namespace glm;
constexpr double SECONDS_PER_FRAME = 1.0 / 60.0;
constexpr double PI = 3.141592653589793238462643383279508334;


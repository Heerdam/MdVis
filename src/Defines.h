#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

#include <glm/ext/matrix_float4x4.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <sstream>
#include <map>
#include <functional>
#include <array>
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>
#include <filesystem>
#include <unordered_map>
#include <memory>
#include <forward_list>
#include <chrono>

#define GLM_FORCE_RADIANS

// -------------------- Configuration --------------------

/*
	Change window resolution here.
*/
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

/*
	0 - no interpolation
	1 - linear interpoltation
	2 - cubic spline interpolation
	Default:		2
*/
#define INTERPOLATION_TYPE 0

/*
	Set this to 0 when using the cubic spline interpolation but it gets stuck. Setting this to 0 will
	move the building of the spline onto the cpu.
	Valid values:	0, 1
	Default:		1
*/
#define USE_SPLINE_SHADER 1

/*
	Defines how many times the icosahedron gets subdivided. More subdivison means smoother surface
	but more vertices to draw. High impact on performance.
	Valid range: [0,n)
	Default and recommended: 2
*/
#define SPHERE_SUBDIVISIONS 3

/*
	Toggles the axis widget.
	Valid values:	0, 1
	Default:		1
*/
#define WIDGET_SHOW 0

#define WIDGET_WIDTH 400
#define WIDGET_HEIGHT 400

/*
	If enabled it will print out an overview every frame.
	Valid values:	0, 1
	Default:		1
*/
#define LOG_FRAMES 1

/*
	Enables/ Disables SSAO (Screen Space Ambient Occlusion). Disabling it will increase performance.
	Valid values:	0, 1
	Default:		1
*/
#define USE_SSAO 1

/*
	Use this values to tweak the SSAO. Use on your own risk.
*/
#define SSAO_KERNEL_SIZE 64
#define SSAO_RADIUS 1.f
#define SSAO_BIAS 0.025f

/*
Set this to "" if needed when debugging in VsCode
*/
const std::string VSC_WORKDIR_OFFSET = "../";

typedef unsigned int uint;
typedef unsigned short ushort;

typedef glm::vec2 Vec2;
typedef glm::vec3 Vec3;
typedef glm::vec4 Vec4;

#define UVX Vec3(1.f, 0.f, 0.f)
#define UVY Vec3(0.f, 1.f, 0.f)
#define UVZ Vec3(0.f, 0.f, 1.f)

typedef glm::mat4 Mat4;

#define PI 3.14159265358979323846


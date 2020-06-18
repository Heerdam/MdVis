#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

#include <glm/ext/vector_uint2.hpp>
#include <glm/ext/vector_uint3.hpp>
#include <glm/ext/vector_uint4.hpp>

#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_int3.hpp>
#include <glm/ext/vector_int4.hpp>

#include <glm/ext/matrix_float4x4.hpp>

#include <glm/gtx/quaternion.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
#include <atomic>
#include <queue>
#include <mutex>
#include <chrono>
#include <filesystem>

#define GLM_FORCE_RADIANS

typedef unsigned int uint;
typedef unsigned short ushort;

typedef glm::ivec2 Vec2i;
typedef glm::ivec3 Vec3i;
typedef glm::ivec4 Vec4i;

typedef glm::uvec2 Vec2u;
typedef glm::uvec3 Vec3u;
typedef glm::uvec4 Vec4u;

typedef glm::vec2 Vec2;
typedef glm::vec3 Vec3;
typedef glm::vec4 Vec4;

#define UVX Vec3(1.f, 0.f, 0.f)
#define UVY Vec3(0.f, 1.f, 0.f)
#define UVZ Vec3(0.f, 0.f, 1.f)

typedef glm::mat3 Mat3;
typedef glm::mat4 Mat4;

#define M00(X) (X[0][0])
#define M01(X) (X[0][1])
#define M02(X) (X[0][2])
#define M03(X) (X[0][3])

#define M10(X) (X[1][0])
#define M11(X) (X[1][1])
#define M12(X) (X[1][2])
#define M13(X) (X[1][3])

#define M20(X) (X[2][0])
#define M21(X) (X[2][1])
#define M22(X) (X[2][2])
#define M23(X) (X[2][3])

#define M30(X) (X[3][0])
#define M31(X) (X[3][1])
#define M32(X) (X[3][2])
#define M33(X) (X[3][3])

typedef glm::quat Quat;

#define CRS(X, Y) (glm::cross((X), (Y)))
#define DOT(X, Y) (glm::dot((X), (Y)))
#define DIS(X, Y) (glm::distance((X), (Y)))
#define LEN(X) (glm::length((X)))
#define NOR(X) (glm::normalize((X)))

#define DET(X) (glm::determinant((X)))
#define INV(X) (glm::inverse((X)))
#define TR(X) (glm::transpose((X)))
#define ToArray(X) (glm::value_ptr((X)))

/*returns an identity 4x4 matrix*/
#define IDENTITY glm::identity<Mat4>()

/*Creates a frustum matrix.*/
#define FRUSTUM(LEFT, RIGHT, BOTTOM, TOP, NEARPLANE, FARPLANE) (gml::frustm((LEFT), (RIGHT), (BOTTOM), (TOP), (NEARPLANE), (FARPLANE)))

/*Creates a matrix for a symmetric perspective-view frustum with far plane at infinite.
FOVY: Expressed in radians if GLM_FORCE_RADIANS is define or degrees otherwise*/
#define INFINITEPERSPECTIVE(FOVY, ASPECT, NEARPLANE) (gml::infinitePerspective((FOVY), (ASPECT), (NEARPLANE)))

/*Build a look at view matrix.
EYE: Position of the camera
CENTER: Position where the camera is looking at
UP: Normalized up vector, how the camera is oriented. Typically (0, 0, 1)*/
#define LOOKAT(EYE, CENTER, UP) (glm::lookAt((EYE), (CENTER), (UP)))

/*Creates a matrix for an orthographic parallel viewing volume.*/
#define ORTHO(LEFT, RIGHT, BOTTOM, TOP, ZNEAR, ZFAR) (glm::ortho((LEFT), (RIGHT), (BOTTOM), (TOP), (ZNEAR), (ZFAR)))

/*Creates a matrix for projecting two-dimensional coordinates onto the screen.*/
#define ORTHO2D(LEFT, RIGHT, BOTTOM, TOP) (glm::ortho((LEFT), (RIGHT), (BOTTOM), (TOP)))

/*Creates a matrix for a symetric perspective-view frustum.
FOVY: Expressed in radians if GLM_FORCE_RADIANS is define or degrees otherwise*/
#define PERSPECTIVE(FOVY, ASPECT, NEARPLANE, FARPLANE) (glm::perspective((FOVY), (ASPECT), (NEARPLANE), (FARPLANE)))

/*Builds a perspective projection matrix based on a field of view.
FOVY: Expressed in radians if GLM_FORCE_RADIANS is define or degrees otherwise*/
#define PERSPECTIVEFOV(FOV, WIDTH, HEIGHT, NEARPLANE, FARPLANE) glm::perspectiveFov((FOV), (WIDTH), (HEIGHT), (NEARPLANE), (FARPLANE))

/*Define a picking region.*/
#define PICKMATRIX(CENTER, DELTA, VIEWPORT) (glm::pickMatrix((CENTER), (DELTA), (VIEWPORT))

/*Map the specified object coordinates (OBJECT.x, OBJECT.y, OBJECT.z) into window coordinates.*/
#define PROJECT(OBJECT, MODEL, PROJ, VIEWPORT) (glm::project((OBJECT), (MODEL), (PROJ), (VIEWPORT)))

/*Map the specified window coordinates (WINDOW.x, WINDOW.y, WINDOW.z) into object coordinates.*/
#define UNPROJECT(WINDOW, MODEL, PROJ, VIEWPORT) (glm::unProject((WINDOW), (MODEL), (PROJ), (VIEWPORT)))

/*Builds a rotation 4 * 4 matrix created from an axis vector and an angle.
	INPUTMATRIX: Input matrix multiplied by this rotation matrix.
	ANGLE: Rotation angle expressed in radians if GLM_FORCE_RADIANS is define or degrees otherwise.
	AXIS: Rotation axis, recommanded to be normalized.*/
#define ROTATEMATRIX(INPUTMATRIX, ANGLE, AXIS) (glm::rotate((INPUTMATRIX), (ANGLE), (AXIS)))

	/*Builds a scale 4 * 4 matrix created from 3 scalars.
		INPUTMATRIX: Input matrix multiplied by this scale matrix.
		SCALERATIO: Ratio of scaling for each axis.
	*/
#define SCALEMATRIX(INPUTMATRIX, SCALERATIO) (glm::scale((INPUTMATRIX), (SCALERATIO)))

	/*Builds a translation 4 * 4 matrix created from a vector of 3 components.
		INPUTMATRIX: Input matrix multiplied by this translation matrix.
		TRANSLATIONVECTOR: Coordinates of a translation vector.*/
#define TRANSLATIONMATRIX(INPUTMATRIX, TRANSLATIONVECTOR) (glm::translate((INPUTMATRIX), (TRANSLATIONVECTOR)))

#define TOMAT4(X) (glm::toMat4((X)))
#define EULER(X) (glm::eulerAngles((X)))
#define ROLL(X) (glm::roll((X)))
#define YAW(X) (glm::yaw((X))))
#define PITCH(X) (glm::pitch((X)))

#define PI 3.14159265358979323846
#define DEGTORAD static_cast<float>(PI / 180.0)
#define TORAD(X) ((X) * DEGTORAD)
#define RADTODEG static_cast<float>(180.0 / PI)
#define TODEG(X) ((X) * RADTODEG)
#define ABS(X) (std::fabs(X))
#define CLAMP(X, MIN, MAX) (std::clamp((X), (MIN), (MAX)))
#define SIN(X) (std::sin(X))
#define ASIN(X) (std::asin(X))
#define COS(X) (std::cos(X))
#define ACOS(X) (std::acos(X))
#define TAN(X) (std::tan(X))
#define ATAN(X) (std::atan(X))
#define INF (std::numeric_limits<float>::infinity())
#define SQRT(X) (std::sqrt((X)))
#define POW(X, Y) (std::pow((X), (Y)))

#define colF(X) ((X)/255.f)


inline void LOG(const std::string _in, std::mutex& _mutex) {
	std::lock_guard<std::mutex> lock(_mutex);
	std::cout << _in << std::endl;
};

inline void LOG(const std::string _in) {
	std::cout << _in << std::endl;
};

inline void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam) {
	if (type != GL_DEBUG_TYPE_ERROR) return;
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

const inline float axisV[234] = {
	-11.748000f, -0.360000f, 1.268000f,
	-11.276000f, -0.360000f, 1.268000f,
	-10.424000f, -0.360000f, 0.228000f,
	-9.568000f, -0.360000f, 1.268000f,
	-9.092000f, -0.360000f, 1.268000f,
	-10.184000f, -0.360000f, -0.048000f,
	-9.008000f, -0.360000f, -1.460000f,
	-9.484000f, -0.360000f, -1.460000f,
	-10.424000f, -0.360000f, -0.332000f,
	-11.352000f, -0.360000f, -1.460000f,
	-11.824000f, -0.360000f, -1.460000f,
	-10.660000f, -0.360000f, -0.048000f,
	-9.484000f, 0.373033f, -1.460000f,
	-10.424000f, 0.373033f, -0.332000f,
	-11.748000f, 0.373033f, 1.268000f,
	-10.660000f, 0.373033f, -0.048000f,
	-9.568000f, 0.373033f, 1.268000f,
	-10.184000f, 0.373033f, -0.048000f,
	-11.824000f, 0.373033f, -1.460000f,
	-9.008000f, 0.373033f, -1.460000f,
	-10.424000f, 0.373033f, 0.228000f,
	-11.352000f, 0.373033f, -1.460000f,
	-9.092000f, 0.373033f, 1.268000f,
	-11.276000f, 0.373033f, 1.268000f,
	-0.620000f, -0.380000f, 11.376000f,
	1.116000f, -0.380000f, 9.000000f,
	-1.344000f, -0.380000f, 9.000000f,
	-1.344000f, -0.380000f, 9.352000f,
	0.392000f, -0.380000f, 9.352000f,
	-1.344000f, -0.380000f, 11.728000f,
	1.008000f, -0.380000f, 11.728000f,
	1.008000f, -0.380000f, 11.376000f,
	0.392000f, 0.384923f, 9.352000f,
	-1.344000f, 0.384923f, 11.728000f,
	-0.620000f, 0.384923f, 11.376000f,
	-1.344000f, 0.384923f, 9.352000f,
	1.116000f, 0.384923f, 9.000000f,
	1.008000f, 0.384923f, 11.728000f,
	1.008000f, 0.384923f, 11.376000f,
	-1.344000f, 0.384923f, 9.000000f,
	-0.250000f, 10.788000f, 0.104000f,
	-0.250000f, 11.728000f, -0.628000f,
	-0.250000f, 11.728000f, -1.100000f,
	-0.250000f, 10.444000f, -0.088000f,
	-0.250000f, 9.000000f, -0.088000f,
	-0.250000f, 9.000000f, 0.304000f,
	-0.250000f, 10.440000f, 0.304000f,
	-0.250000f, 11.728000f, 1.316000f,
	-0.250000f, 11.728000f, 0.844000f,
	0.304321f, 9.000000f, -0.088000f,
	0.304321f, 9.000000f, 0.304000f,
	0.304321f, 10.440000f, 0.304000f,
	0.304321f, 10.444000f, -0.088000f,
	0.304321f, 11.728000f, 1.316000f,
	0.304321f, 10.788000f, 0.104000f,
	0.304321f, 11.728000f, -0.628000f,
	0.304321f, 11.728000f, -1.100000f,
	0.304321f, 11.728000f, 0.844000f,
	0.500000f, -0.500000f, -0.500000f,
	0.500000f, 0.500000f, -0.500000f,
	0.500000f, -0.500000f, 0.500000f,
	0.500000f, 0.500000f, 0.500000f,
	-0.500000f, -0.500000f, -0.500000f,
	-0.500000f, 0.500000f, -0.500000f,
	-0.500000f, -0.500000f, 0.500000f,
	-0.500000f, 0.500000f, 0.500000f,
	-8.500000f, 0.500000f, 0.500000f,
	-8.500000f, -0.500000f, 0.500000f,
	-8.500000f, -0.500000f, -0.500000f,
	-8.500000f, 0.500000f, -0.500000f,
	0.500000f, 8.500000f, -0.500000f,
	0.500000f, 8.500000f, 0.500000f,
	-0.500000f, 8.500000f, 0.500000f,
	-0.500000f, 8.500000f, -0.500000f,
	0.500000f, 0.500000f, 8.500000f,
	0.500000f, -0.500000f, 8.500000f,
	-0.500000f, -0.500000f, 8.500000f,
	-0.500000f, 0.500000f, 8.500000f
};

const inline uint axisI[420] = {
	6, 5, 4,
	6, 4, 3,
	3, 2, 1,
	3, 1, 12,
	6, 3, 12,
	7, 6, 12,
	7, 12, 9,
	9, 12, 11,
	7, 9, 8,
	10, 9, 11,
	18, 17, 23,
	18, 21, 17,
	21, 15, 24,
	21, 16, 15,
	18, 16, 21,
	20, 16, 18,
	20, 14, 16,
	14, 19, 16,
	20, 13, 14,
	22, 19, 14,
	6, 23, 5,
	9, 13, 8,
	10, 14, 9,
	7, 18, 6,
	1, 16, 12,
	11, 22, 10,
	2, 15, 1,
	5, 17, 4,
	4, 21, 3,
	3, 24, 2,
	8, 20, 7,
	12, 19, 11,
	32, 31, 30,
	32, 30, 25,
	25, 30, 29,
	26, 25, 29,
	26, 29, 28,
	26, 28, 27,
	39, 34, 38,
	39, 35, 34,
	35, 33, 34,
	37, 33, 35,
	37, 36, 33,
	37, 40, 36,
	28, 40, 27,
	25, 39, 32,
	32, 38, 31,
	26, 35, 25,
	29, 36, 28,
	30, 33, 29,
	27, 37, 26,
	31, 34, 30,
	44, 42, 43,
	44, 41, 42,
	41, 48, 49,
	41, 47, 48,
	44, 47, 41,
	45, 47, 44,
	45, 46, 47,
	53, 57, 56,
	53, 56, 55,
	55, 58, 54,
	55, 54, 52,
	53, 55, 52,
	50, 53, 52,
	50, 52, 51,
	58, 41, 49,
	50, 46, 45,
	55, 42, 41,
	54, 49, 48,
	51, 47, 46,
	52, 48, 47,
	56, 43, 42,
	57, 44, 43,
	53, 45, 44,
	60, 61, 59,
	65, 78, 66,
	66, 68, 65,
	64, 59, 63,
	65, 59, 61,
	60, 72, 62,
	67, 69, 68,
	63, 70, 64,
	65, 69, 63,
	64, 67, 66,
	72, 74, 73,
	64, 71, 60,
	62, 73, 66,
	66, 74, 64,
	75, 77, 76,
	61, 77, 65,
	66, 75, 62,
	62, 76, 61,
	6, 18, 23,
	9, 14, 13,
	10, 22, 14,
	7, 20, 18,
	1, 15, 16,
	11, 19, 22,
	2, 24, 15,
	5, 23, 17,
	4, 17, 21,
	3, 21, 24,
	8, 13, 20,
	12, 16, 19,
	28, 36, 40,
	25, 35, 39,
	32, 39, 38,
	26, 37, 35,
	29, 33, 36,
	30, 34, 33,
	27, 40, 37,
	31, 38, 34,
	58, 55, 41,
	50, 51, 46,
	55, 56, 42,
	54, 58, 49,
	51, 52, 47,
	52, 54, 48,
	56, 57, 43,
	57, 53, 44,
	53, 50, 45,
	60, 62, 61,
	65, 77, 78,
	66, 67, 68,
	64, 60, 59,
	65, 63, 59,
	60, 71, 72,
	67, 70, 69,
	63, 69, 70,
	65, 68, 69,
	64, 70, 67,
	72, 71, 74,
	64, 74, 71,
	62, 72, 73,
	66, 73, 74,
	75, 78, 77,
	61, 76, 77,
	66, 78, 75,
	62, 75, 76
};
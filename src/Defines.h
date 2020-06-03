#pragma once

#include "../libs/glew-2.1.0/include/GL/glew.h"

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

#define UVX Vec4(1.f, 0.f, 0.f, 1.f)
#define UVY Vec4(0.f, 1.f, 0.f, 1.f)
#define UVZ Vec4(0.f, 0.f, 1.f, 1.f)

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

#define PI 3.14159265358979323846f
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

/*
 ++ DEPRECATED ++
inline void GLError(std::string _id) {
	bool hasError = false;
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		if (!hasError) {
			hasError = true;
			std::cout << "---- Print OpenGl Errors: " << _id << " ----" << std::endl;
		}
		switch (err) {
		case GL_INVALID_VALUE:
			std::cout << "Invalid Value";
			break;
		case GL_INVALID_OPERATION:
			std::cout << "Invalid Operation";
			break;
		case GL_OUT_OF_MEMORY:
			std::cout << "Out of Memory";
			break;
		case GL_INVALID_ENUM:
			std::cout << "Invalid Enum";
			break;
		case GL_STACK_OVERFLOW:
			std::cout << "Stack Overflow";
			break;
		case GL_STACK_UNDERFLOW:
			std::cout << "Stack Underflow";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "Invalid Framebuffer Operation";
			break;
		//case GL_CONTEXT_LOST:
		//	std::cout << "Context Lost";
		//	break;
		}
		std::cout << " (" << err << ")" << std::endl;
	}
	if (hasError) std::cout << "---- Finished ----" << std::endl;
};

inline void printFrameBufferErrors(std::string _id) {
	switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
	case GL_FRAMEBUFFER_COMPLETE:
		return;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cout << "---- Print Framebuffer Errors: " << _id << " ----" << std::endl;
		std::cout << "FRAMEBUFFER INCOMPLETE ATTACHMENT" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cout << "---- Print Framebuffer Errors: " << _id << " ----" << std::endl;
		std::cout << "FRAMEBUFFER INCOMPLETE MISSING ATTACHMENT" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cout << "---- Print Framebuffer Errors: " << _id << " ----" << std::endl;
		std::cout << "FRAMEBUFFER INCOMPLETE DRAW BUFFER" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cout << "---- Print Framebuffer Errors: " << _id << " ----" << std::endl;
		std::cout << "FRAMEBUFFER INCOMPLETE READ BUFFER" << std::endl;
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cout << "---- Print Framebuffer Errors: " << _id << " ----" << std::endl;
		std::cout << "FRAMEBUFFER UNSUPPORTED" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		std::cout << "---- Print Framebuffer Errors: " << _id << " ----" << std::endl;
		std::cout << "FRAMEBUFFER INCOMPLETE MULTISAMPLE" << std::endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		std::cout << "---- Print Framebuffer Errors: " << _id << " ----" << std::endl;
		std::cout << "FRAMEBUFFER INCOMPLETE LAYER TARGETS" << std::endl;
		break;
	}
	std::cout << "---- Finished ----" << std::endl;
};

*/

inline void LOG(const std::string _in) {
	std::cout << _in << std::endl;
};
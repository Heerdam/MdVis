
#include "Defines.h"

class ShaderProgram {

	enum class Status {
		success, failed, missing
	};

	const std::string id;

	GLint program = -1, compute = -1, vertex = -1, geom = -1, frag = -1;
	void print(std::string, Status, Status, Status, Status, Status, std::string);
	

public:
	ShaderProgram(std::string);
	ShaderProgram();
	~ShaderProgram();

	bool printDebug = true;

	/*
	assumes the following:
	compute shader: [PATH_TO_FILE].comp
	vertex shader: [PATH_TO_FILE].ver
	geometry shader: [PATH_TO_FILE].geo
	fragment shader: [PATH_TO_FILE].frag
	*/
	bool compileFromFile(const std::string&);
	bool compile(const char*, const char*, const char*, const char*);
	GLuint getHandle();
	void bind();
	void unbind();
};

class SSBO {

	const std::string id;

	GLuint handle;
	GLbitfield flags;
	uint lastBindTarget;
	void* pntr;

public:
	/*
	https://www.khronos.org/opengl/wiki/GLAPI/glBufferStorage
	GL_DYNAMIC_STORAGE_BIT, GL_MAP_READ_BIT, GL_MAP_WRITE_BIT, GL_MAP_PERSISTENT_BIT,
	GL_MAP_COHERENT_BIT, GL_CLIENT_STORAGE_BIT
	*/
	SSBO(std::string, uint, void*, GLbitfield);
	~SSBO();

	void bind(uint);
	void bindAs(uint, uint);
	void unbind();
	/*
	https://www.khronos.org/opengl/wiki/GLAPI/glMapBufferRange
	!offset and length in byte!
	GL_MAP_READ_BIT, GL_MAP_WRITE_BIT, GL_MAP_INVALIDATE_RANGE_BIT,
	GL_MAP_INVALIDATE_BUFFER_BIT, GL_MAP_FLUSH_EXPLICIT_BIT,
	GL_MAP_UNSYNCHRONIZED_BIT, GL_MAP_PERSISTENT_BIT,
	GL_MAP_COHERENT_BIT
	*/
	void* map(uint, uint, GLbitfield);
	void unmap();

	uint getHandle() {
		return handle;
	}

	template<class T>
	T inline getPtr() {
		return reinterpret_cast<T>(pntr);
	};

};

class VBO {
	const std::string id;
	GLuint handle;
	uint size; //indices

public:
	/*
	https://www.khronos.org/opengl/wiki/GLAPI/glBufferData
	usage: GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW,
	GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY.
	*/
	VBO(std::string, GLsizeiptr, const GLvoid*, GLenum, uint);
	~VBO();

	void bind();
	void unbind();

	void bindAs(uint, uint);

	void drawAll(GLenum);
	void draw(GLenum, GLint, GLsizei);

	/*
	https://www.khronos.org/opengl/wiki/GLAPI/glVertexAttribPointer
	call bind() first and unbind() afterwards
	*/
	void enableVertexAttribute(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*);

	GLuint getHandle() {
		return handle;
	}
};

class EBO {
	const std::string id;
	GLuint handle;
	uint size; //indices

public:
	/*
	https://www.khronos.org/opengl/wiki/GLAPI/glBufferData
	usage: GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW,
	GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY.
	*/
	EBO(std::string, GLsizeiptr, const GLvoid*, GLenum, uint);
	~EBO();

	void bind();
	void unbind();

	void drawAll(GLenum);
	void draw(GLenum, GLint, GLenum, const GLvoid*);

	GLuint getHandle() {
		return handle;
	}

};

class Camera {

public:

	Camera(const float, const float);

	Vec4 position;
	Vec4 direction;
	Vec4 up;
	Vec4 right;

	Mat4 projection;
	Mat4 view;
	Mat4 combined;

	float fieldOfView = 67.f;
	float nearPlane = 1.0f;
	float farPlane = 500.f;
	float viewportWidth = 0.f;
	float viewportHeight = 0.f;

	Vec3 target;
	float azimuth = 0.f, height = 90.f, distance = 100.f;

	void update();

	void lookAt(const Vec3&);
	void normalizeUp();
	void normalizeUpAxisLocked(const Vec3&);

	void arcball(const Vec3&, const float, const float, const float);

	void translate(const Vec3&);

};

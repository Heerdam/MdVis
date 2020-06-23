
#include "Defines.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#endif

class ShaderProgram {

	enum class Status {
		success, failed, missing
	};

	GLint program = -1, compute = -1, vertex = -1, geom = -1, frag = -1;
	void print(std::string, Status, Status, Status, Status, Status, std::string);
	

public:
	ShaderProgram(std::string);
	ShaderProgram();
	~ShaderProgram();

	std::string id;
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

class Camera {

	bool isOrtho = false;

public:
	Camera() {};
	Camera(bool, const float, const float);

	Vec3 position;
	Vec3 direction;
	Vec3 up;
	Vec3 right;

	Mat4 projection;
	Mat4 view;
	Mat4 combined;

	float fieldOfView = 67.f;
	float nearPlane = 0.0001f;
	float farPlane = 5000.f;
	float viewportWidth = 0.f;
	float viewportHeight = 0.f;

	Vec3 target;
	float azimuth = 0.f, height = 90.f, distance = 100.f;
	
	void update();

	void lookAt(const Vec3&);
	void normalizeUp();
	void normalizeUpAxisLocked(const Vec3&);
	void arcball();

};

class CameraController {
	Camera* camera;

	float oldX = -1.f, oldY = -1.f;
	bool mbDown = false;

public:
	float velocity = 5.f;
	float degreesPerPixel = 0.5f;

	CameraController() {};
	CameraController(Camera*);

	void mbCB(int, int, int);
	void cursorCB(double, double);
	void update(GLFWwindow* , float);
};

struct Icosahedron {
	static std::pair<std::vector<float>, std::vector<uint>>create(uint);
};

#define USE_BINARY 1

struct FileParser {
	static void loadFile(std::string _path, std::vector<float>& _coords, uint& _count, Vec3& _low, Vec3& _up, Vec3& _dims);
};

class Logger {

	Logger() {};

	static Logger* instance;

	std::mutex mutex;
	std::chrono::high_resolution_clock::time_point start;

public:
	static void init();
	static void LOG(const std::string&, bool);
	static Logger* get();
};

#if USE_SPLINE_SHADER 
struct SplineBuilder {
	static void build(uint _count, uint _steps, const Vec3& dims, std::vector<float>& _traj, std::vector<float>& _out);
};
#endif // USE_SPLINE_SHADER

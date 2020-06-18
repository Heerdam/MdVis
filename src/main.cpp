
#include "GL.h"

#include <glm/gtx/string_cast.hpp>

#include "xoshiro.h"

#include <sstream>
#include <map>
#include <functional>
#include <array>

struct Proxy {
	// -------------------- File --------------------
	std::string pathToFile;

	// -------------------- States --------------------
	GLFWwindow* window;
	Camera cam, widgetCam;
	CameraController controller;
	Vec3 dims;
	Vec3 low, up;

	// -------------------- Viewport --------------------
	int wWidth, wHeight;
	int widgetWidth = 400, widgetHeight = 400;

	// -------------------- Inputs --------------------
	bool rmb_down = false;
	double oldX = -1.f, oldY = -1.f;
	float t = 0.f, deltaT = 0.00001f;

	// -------------------- Constants --------------------
	uint ATOMCOUNT, TIMESTEPS, SPHEREVERTICES, INDEXCOUNT;
	const uint VERTEXSIZE = 3u; //pos
	const uint SPHEREVERTEXSIZE = 3u; //pos
	const uint AUXVERTEXSIZE = 3u + 3u + 3u; //nrm + t + bt
	const uint GUI_MAX_VERTICES = 6000u;
	const uint GUI_MAX_IDX = 6000u;

	// -------------------- GL --------------------
	Vec4 clearColor = Vec4(0.03f, 0.09f, 0.22f, 1.f); 
	Vec4 atomColor = Vec4(0.73f, 0.84f, 0.99f, 1.f);
	bool isGLloaded = false, shouldTerminate = false;

	//shaders
	ShaderProgram splineShader, compShader, geomShader, lightShader, widgetShader/*, guiShader*/;

	//compute pass
	GLuint c_ssbo_traj, c_ssbo_sphere, cg_vbo, c_ssbo_weights;

	//geometry pass
	GLuint g_vao, g_fb, g_pos, g_nrm, g_t, g_bt, g_col, g_depth, g_vbo_aux, g_ebo;

	//light pass
	std::vector<float> lights;
	GLuint l_vao;

	//forward pass
	GLuint widget_vao;

	// -------------------- Data --------------------
	std::vector<float> coords, newTraj, sphere_vertices, auxBuffer;
	std::vector<uint> sphere_indices;

	// -------------------- Queue --------------------
	std::mutex mutex, logMutex;
	std::queue<std::function<void(Proxy*)>> asyncQueue;
};

void load(std::atomic<float>& _progress, Proxy& _proxy) {

	{
		std::lock_guard<std::mutex> lock(_proxy.mutex);
		_proxy.asyncQueue.push([](Proxy* _proxy)->void {
			//COMPILE SHADERS
			//TODO schäider für 2 cases
			_proxy->compShader.id = "c_shader";
			_proxy->compShader.compileFromFile(std::filesystem::absolute(std::filesystem::path("../shader/c_shader")).string());
			_proxy->geomShader.id = "g_shader";
			_proxy->geomShader.compileFromFile(std::filesystem::absolute(std::filesystem::path("../shader/g_shader")).string());
			_proxy->lightShader.id = "l_shader";
			_proxy->lightShader.compileFromFile(std::filesystem::absolute(std::filesystem::path("../shader/l_shader")).string());
			_proxy->widgetShader.id = "widget_shader";
			_proxy->widgetShader.compileFromFile(std::filesystem::absolute(std::filesystem::path("../shader/widget_shader")).string());
		});
	}

	LOG("Loading file: " + _proxy.pathToFile + "\n", _proxy.logMutex);

	//PARSE FILE
	FileParser::parse(std::filesystem::absolute(std::filesystem::path("../out1.traj")).string(), _proxy.coords, _proxy.ATOMCOUNT, _proxy.low, _proxy.up, _proxy.dims);
	//FileParser::parse(filesystem::path(_proxy.pathToFile).make_absolute().str(), _proxy.coords, _proxy.ATOMCOUNT);
	_proxy.TIMESTEPS = static_cast<uint>(_proxy.coords.size() / 3) / _proxy.ATOMCOUNT;

	LOG("Atoms: " + std::to_string(_proxy.ATOMCOUNT) + " Steps: " + std::to_string(_proxy.TIMESTEPS) + "\n", _proxy.logMutex);
	LOG("Bounds: [" + std::to_string(_proxy.up.x) + ", " + std::to_string(_proxy.up.y) + ", " + std::to_string(_proxy.up.z) + "]\n", _proxy.logMutex);

	{
		std::lock_guard<std::mutex> lock(_proxy.mutex);
		_proxy.asyncQueue.push([](Proxy* _proxy)->void {
			glGenBuffers(1, &_proxy->c_ssbo_traj);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _proxy->c_ssbo_traj);
			glBufferData(GL_SHADER_STORAGE_BUFFER, _proxy->coords.size() * sizeof(float), _proxy->coords.data(), GL_DYNAMIC_DRAW);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			glGenBuffers(1, &_proxy->cg_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, _proxy->cg_vbo);
			glBufferData(GL_ARRAY_BUFFER, _proxy->ATOMCOUNT * _proxy->VERTEXSIZE * _proxy->SPHEREVERTICES * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		});
	}

	{
		Vec3 cntr;
		for (uint i = 0; i < 3; ++i)
			cntr[i] = (_proxy.up[i] - _proxy.low[i]) / 2.f + _proxy.low[i];

		//SET UP THE CAMERAS
		_proxy.cam = Camera(false, _proxy.wWidth, _proxy.wHeight);
		_proxy.cam.fieldOfView = 50.f;
		_proxy.cam.nearPlane = 0.001f;
		_proxy.cam.farPlane = 50000.f;
		_proxy.cam.position = Vec3(cntr[0] + 1.2f * _proxy.dims[0], cntr[1], cntr[2]);
		_proxy.cam.direction = NOR(cntr - _proxy.cam.position);
		_proxy.cam.up = Vec3(0.f, 1.f, 0.f);
		_proxy.cam.height = 0.1f;
		_proxy.cam.update();

		_proxy.widgetCam = Camera(false, 400.f, 400.f);
		_proxy.widgetCam.nearPlane = 1.f;
		_proxy.widgetCam.farPlane = 10000.f;
		_proxy.widgetCam.position = Vec3(-50.f, 0.f, 0.f);
		_proxy.widgetCam.direction = Vec3(1.f, 0.f, 0.f);
		_proxy.widgetCam.up = Vec3(0.f, 1.f, 0.f);
		_proxy.widgetCam.update();

		_proxy.controller = CameraController(&_proxy.cam);
	}
	
	//CREATE SPHERE
	const uint subdivisons = 3;
	auto sphere = Icosahedron::create(subdivisons);
	_proxy.sphere_vertices = std::get<0>(sphere);
	auto& sphere_indices = std::get<1>(sphere);

	LOG("Icosahedron loaded with " + std::to_string(subdivisons) + " subdivisons [i: " + std::to_string(sphere_indices.size()) + ", v: " + std::to_string(_proxy.sphere_vertices.size()) + "]", _proxy.logMutex);

	//MERGE INDICES OF SPHERES
	size_t indexSize = sphere_indices.size();
	uint maxIndex = *std::max_element(sphere_indices.begin(), sphere_indices.end()) + 1;
	_proxy.sphere_indices.resize(indexSize * _proxy.ATOMCOUNT);

	for (size_t s = 0; s < _proxy.ATOMCOUNT; ++s)
		for (size_t i = 0; i < indexSize; ++i)
			_proxy.sphere_indices[s * indexSize + i] = sphere_indices[i] + s * maxIndex;

	//GL CONSTANTS
	_proxy.SPHEREVERTICES = _proxy.sphere_vertices.size() / _proxy.SPHEREVERTEXSIZE;
	_proxy.INDEXCOUNT = _proxy.sphere_indices.size();

	LOG("Indices merged for " + std::to_string(_proxy.ATOMCOUNT) + " atoms: " + std::to_string(_proxy.INDEXCOUNT) + "\n", _proxy.logMutex);

	{
		std::lock_guard<std::mutex> lock(_proxy.mutex);
		_proxy.asyncQueue.push([](Proxy* _proxy)->void {
			glGenBuffers(1, &_proxy->c_ssbo_sphere);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _proxy->c_ssbo_sphere);
			glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<uint>(_proxy->sphere_vertices.size()) * sizeof(float), _proxy->sphere_vertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			glGenBuffers(1, &_proxy->g_ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _proxy->g_ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _proxy->INDEXCOUNT * sizeof(uint), _proxy->sphere_indices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		});
	}

	//CREATE AUX BUFFER
	_proxy.auxBuffer.resize(_proxy.ATOMCOUNT * _proxy.SPHEREVERTICES * _proxy.AUXVERTEXSIZE);
	for (uint i = 0; i < _proxy.SPHEREVERTICES; ++i) {
		//nrm 
		Vec3 v = Vec3(_proxy.sphere_vertices[3 * i], _proxy.sphere_vertices[3 * i + 1], _proxy.sphere_vertices[3 * i + 2]);
		std::memcpy(_proxy.auxBuffer.data() + i * _proxy.AUXVERTEXSIZE, glm::value_ptr(v), 3 * sizeof(float));
		//t
		Vec3 t = NOR(CRS(v, Vec3(0.f, 1.f, 0.f)));		
		if (std::isnan(t[0]) || std::isnan(t[1]) || std::isnan(t[2]))
			t = NOR(CRS(v, Vec3(1.f, 0.f, 0.f)));
		std::memcpy(_proxy.auxBuffer.data() + i * _proxy.AUXVERTEXSIZE + 3, glm::value_ptr(t), 3 * sizeof(float));
		//bt
		Vec3 bt = NOR(CRS(v, t));
		std::memcpy(_proxy.auxBuffer.data() + i * _proxy.AUXVERTEXSIZE + 6, glm::value_ptr(bt), 3 * sizeof(float));
	}

	for (uint i = 1; i < _proxy.ATOMCOUNT; ++i) {
		std::memcpy(_proxy.auxBuffer.data() + i * (_proxy.SPHEREVERTICES * _proxy.AUXVERTEXSIZE), _proxy.auxBuffer.data(), _proxy.SPHEREVERTICES * _proxy.AUXVERTEXSIZE * sizeof(float));
	}

	LOG("Normals, Tangents and Bitangents created.\n", _proxy.logMutex);

	{
		std::lock_guard<std::mutex> lock(_proxy.mutex);
		_proxy.asyncQueue.push([](Proxy* _proxy)->void {
			glGenBuffers(1, &_proxy->g_vbo_aux);
			glBindBuffer(GL_ARRAY_BUFFER, _proxy->g_vbo_aux);
			glBufferData(GL_ARRAY_BUFFER, _proxy->auxBuffer.size() * sizeof(float), _proxy->auxBuffer.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		});
	}

	if (_proxy.TIMESTEPS > 1) {

		{
			std::lock_guard<std::mutex> lock(_proxy.mutex);
			_proxy.asyncQueue.push([](Proxy* _proxy)->void {
				//return;
				glGenBuffers(1, &_proxy->c_ssbo_weights);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _proxy->c_ssbo_weights);
				glBufferData(GL_SHADER_STORAGE_BUFFER, _proxy->ATOMCOUNT*12*sizeof(float)*_proxy->TIMESTEPS, nullptr, GL_STATIC_DRAW);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

				ShaderProgram shader;
				shader.id = "t_shader";
				shader.compileFromFile(std::filesystem::absolute(std::filesystem::path("../shader/t_shader")).string());
				shader.bind();

				//buffers
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _proxy->c_ssbo_traj);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _proxy->c_ssbo_weights);

				//uniforms
				glUniform1ui(1, _proxy->ATOMCOUNT);
				glUniform1ui(2, _proxy->TIMESTEPS);
				glUniform3fv(3, 1, glm::value_ptr(_proxy->dims));

				glDispatchCompute(_proxy->ATOMCOUNT, 1, 1);

				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);

				shader.unbind();

				glMemoryBarrier(GL_ALL_BARRIER_BITS);

				/*
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, _proxy->c_ssbo_weights);
				float* data = new float[_proxy->ATOMCOUNT * 4 * sizeof(float) * _proxy->TIMESTEPS];
				glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _proxy->ATOMCOUNT * 4 * sizeof(float) * _proxy->TIMESTEPS, data);

				for (uint i = 0; i < _proxy->ATOMCOUNT * 4 * sizeof(float) * _proxy->TIMESTEPS; i+=4) {
					std::cout << i << " " << data[i] << " " << data[i+1] << " " << data[i+2] << " " << data[i+3] << std::endl;
						
				}
				*/

				//for (uint i = 0; i < 20; ++i) {
				//	for (uint j = 1; j < 3; ++j) {
				//		for(uint k = 0; k < 3; ++k)
				//			std::cout << "[" << i << "|" << j << "|" << k << "] " << 12 * i + (j - 1) * _proxy->ATOMCOUNT * 12 + k << std::endl;
				//	}
				//}

				
			});
		}
	}

	//Lights
	//l1
	_proxy.lights.emplace_back(-1000.f);
	_proxy.lights.emplace_back(1000.f);
	_proxy.lights.emplace_back(-1000.f);

	_proxy.lights.emplace_back(1.f);
	_proxy.lights.emplace_back(1.f);
	_proxy.lights.emplace_back(1.f);
	//l2
	_proxy.lights.emplace_back(-1000.f);
	_proxy.lights.emplace_back(1000.f);
	_proxy.lights.emplace_back(1000.f);

	_proxy.lights.emplace_back(1.f);
	_proxy.lights.emplace_back(1.f);
	_proxy.lights.emplace_back(1.f);
	//l3
	_proxy.lights.emplace_back(1000.f);
	_proxy.lights.emplace_back(1000.f);
	_proxy.lights.emplace_back(1000.f);

	_proxy.lights.emplace_back(1.f);
	_proxy.lights.emplace_back(1.f);
	_proxy.lights.emplace_back(1.f);
	//l4
	_proxy.lights.emplace_back(1000.f);
	_proxy.lights.emplace_back(1000.f);
	_proxy.lights.emplace_back(-1000.f);

	_proxy.lights.emplace_back(1.f);
	_proxy.lights.emplace_back(1.f);
	_proxy.lights.emplace_back(1.f);

	{
		std::lock_guard<std::mutex> lock(_proxy.mutex);
		_proxy.asyncQueue.push([](Proxy* _proxy)->void {
			//CREATE VAO FOR DRAWING
			glGenVertexArrays(1, &_proxy->g_vao);
			glBindVertexArray(_proxy->g_vao);

			glBindBuffer(GL_ARRAY_BUFFER, _proxy->cg_vbo);

			//pos
			glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, _proxy->VERTEXSIZE * sizeof(float), (void*)0);
			glEnableVertexAttribArray(5);

			glBindBuffer(GL_ARRAY_BUFFER, _proxy->g_vbo_aux);

			//nrm
			glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, _proxy->AUXVERTEXSIZE * sizeof(float), (void*)0);
			glEnableVertexAttribArray(6);
			//tangent
			glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, _proxy->AUXVERTEXSIZE * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(7);
			//bitangent
			glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, _proxy->AUXVERTEXSIZE * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(8);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _proxy->g_ebo);

			glBindVertexArray(0);
		});
	}

	{
		std::lock_guard<std::mutex> lock(_proxy.mutex);
		_proxy.asyncQueue.push([](Proxy* _proxy)->void {
			glGenFramebuffers(1, &_proxy->g_fb);
			glBindFramebuffer(GL_FRAMEBUFFER, _proxy->g_fb);

			//pos
			glGenTextures(1, &_proxy->g_pos);
			glBindTexture(GL_TEXTURE_2D, _proxy->g_pos);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _proxy->wWidth, _proxy->wHeight, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _proxy->g_pos, 0);

			//nrm
			glGenTextures(1, &_proxy->g_nrm);
			glBindTexture(GL_TEXTURE_2D, _proxy->g_nrm);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _proxy->wWidth, _proxy->wHeight, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _proxy->g_nrm, 0);

			//t
			glGenTextures(1, &_proxy->g_t);
			glBindTexture(GL_TEXTURE_2D, _proxy->g_t);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _proxy->wWidth, _proxy->wHeight, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _proxy->g_t, 0);

			//bt
			glGenTextures(1, &_proxy->g_bt);
			glBindTexture(GL_TEXTURE_2D, _proxy->g_bt);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _proxy->wWidth, _proxy->wHeight, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _proxy->g_bt, 0);

			//col
			glGenTextures(1, &_proxy->g_col);
			glBindTexture(GL_TEXTURE_2D, _proxy->g_col);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _proxy->wWidth, _proxy->wHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, _proxy->g_col, 0);

			uint attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
			glDrawBuffers(5, attachments);
			
			//depth
			glGenRenderbuffers(1, &_proxy->g_depth);
			glBindRenderbuffer(GL_RENDERBUFFER, _proxy->g_depth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _proxy->wWidth, _proxy->wHeight);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _proxy->g_depth);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _proxy->g_depth);
	
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				LOG("Framebuffer not complete! Shutting down...", _proxy->logMutex);
				_proxy->shouldTerminate = true;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		});
	}

	{
		std::lock_guard<std::mutex> lock(_proxy.mutex);
		_proxy.asyncQueue.push([](Proxy* _proxy)->void {
			uint axisII[420];
			for (uint i = 0; i < 420; ++i)
				axisII[i] = axisI[i] - 1;

			uint vbo_widget;
			glGenBuffers(1, &vbo_widget);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_widget);
			glBufferData(GL_ARRAY_BUFFER, 234 * sizeof(float), axisV, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			uint ebo_widget;
			glGenBuffers(1, &ebo_widget);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_widget);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 420 * sizeof(uint), axisII, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glGenVertexArrays(1, &_proxy->widget_vao);
			glBindVertexArray(_proxy->widget_vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_widget);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_widget);
			glBindVertexArray(0);
		});
	}

	{
		std::lock_guard<std::mutex> lock(_proxy.mutex);
		_proxy.asyncQueue.push([](Proxy* _proxy)->void {
			const float vert[] = {
				-1.f, -1.f, 0.f, 0.f,
				1.f, -1.f, 1.f, 0.f,
				1.f, 1.f, 1.f, 1.f,
				-1.f, 1.f, 0.f, 1.f
			};

			const uint idx[] = {
				0, 1, 2,
				2, 3, 0
			};

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), vert, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			GLuint ebo;
			glGenBuffers(1, &ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint), idx, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glGenVertexArrays(1, &_proxy->l_vao);
			glBindVertexArray(_proxy->l_vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBindVertexArray(0);
		});
	}

	{
		std::lock_guard<std::mutex> lock(_proxy.mutex);
		_proxy.asyncQueue.push([](Proxy* _proxy)->void {
		

			_proxy->coords.clear();
			_proxy->coords.shrink_to_fit();

			_proxy->sphere_vertices.clear();
			_proxy->sphere_vertices.shrink_to_fit();

			_proxy->sphere_indices.clear();
			_proxy->sphere_indices.shrink_to_fit();

			_proxy->auxBuffer.clear();
			_proxy->auxBuffer.shrink_to_fit();
			
			_proxy->isGLloaded = true;
			_proxy->t = 0.f;
			LOG("Loading finished\n\n", _proxy->logMutex);
		});
	}
	
}

int main() {

	Proxy proxy;
	std::atomic<float> progress;

	// --------------------INITIALISE THE WINDOW, proxy, CAMERAS AND CONTEXT --------------------
	if (!glfwInit()) {
		LOG("GLFW not ok", proxy.logMutex);
		return -1;
	}

	//tschegg ob comp shader awäsend

	glfwWindowHint(GLFW_REFRESH_RATE, 60);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);

	proxy.window = glfwCreateWindow(1920, 1080, "MdVis 0.2a", NULL, NULL);
	if (!proxy.window) {
		glfwTerminate();
		return -1;
	}

	
	glfwMakeContextCurrent(proxy.window);

	glfwGetFramebufferSize(proxy.window, &proxy.wWidth, &proxy.wHeight);
	glViewport(0, 0, proxy.wWidth, proxy.wHeight);
	glfwSwapInterval(0);

	glewExperimental = GL_TRUE;
	if (!glewInit() == GLEW_OK) {
		LOG("glew not ok", proxy.logMutex);
		return -1;
	}

	if (!GL_COMPUTE_SHADER && GL_MAX_COMPUTE_WORK_GROUP_SIZE > 1024) {
		LOG("Compute Shader not supported!", proxy.logMutex);
		return -1;
	}


	//TODO 	LOG("Max Dispatch groups: " + std::to_string(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS) + "\n", _proxy.logMutex);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	// -------------------- SET UP CALLBACKS --------------------
	
	glfwSetWindowUserPointer(proxy.window, &proxy);
	/*
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* _window, int _width, int _height)->void {
		proxy* proxy = reinterpret_cast<proxy*>(glfwGetWindowUserPointer(_window));
		glViewport(0, 0, _width, _height);
		proxy->cam->viewportWidth = _width;
		proxy->cam->viewportWidth = _height;
		proxy->cam->update();
	});
	
*/	glfwSetKeyCallback(proxy.window, [](GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)->void {	
		Proxy* proxy = reinterpret_cast<Proxy*>(glfwGetWindowUserPointer(_window));
		if (!proxy->isGLloaded) return;
		if (_key == GLFW_KEY_SPACE)
			proxy->t = 0.f;
	});
	
	glfwSetCursorPosCallback(proxy.window, [](GLFWwindow* _window, double _xpos, double _ypos)->void {
		Proxy* proxy = reinterpret_cast<Proxy*>(glfwGetWindowUserPointer(_window));
		if (!proxy->isGLloaded) return;
		proxy->controller.cursorCB(_xpos, _ypos);
	});

	glfwSetMouseButtonCallback(proxy.window, [](GLFWwindow* _window, int _button, int _action, int _mods)->void {
		Proxy* proxy = reinterpret_cast<Proxy*>(glfwGetWindowUserPointer(_window));
		if (!proxy->isGLloaded) return;
		proxy->controller.mbCB(_button, _action, _mods);
	});

	std::thread async(&load, std::ref(progress), std::ref(proxy));

	double time = glfwGetTime();
	unsigned long long frame = 0;

	GLsync sync[2];
	uint index = 0;
	sync[0] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	sync[1] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

	while (!glfwWindowShouldClose(proxy.window)) {

		double ctime = glfwGetTime();

		if (!proxy.isGLloaded) {
			{
				std::lock_guard<std::mutex> lock(proxy.mutex);
				if (!proxy.asyncQueue.empty()) {
					proxy.asyncQueue.front()(&proxy);
					proxy.asyncQueue.pop();
				}
			}
		}

		glStencilMask(~0);
		glClearColor(proxy.clearColor[0], proxy.clearColor[1], proxy.clearColor[2], proxy.clearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glfwPollEvents();	

		//DRAW STUFF
		glViewport(0, 0, proxy.wWidth, proxy.wHeight);

		bool show_demo_window = true;

		//create vertexbuffer
		if (proxy.isGLloaded) {

			proxy.controller.update(proxy.window, 1.f / 60.f);
			
			// -------------------- Compute Pass --------------------
			proxy.compShader.bind();

			//buffers
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, proxy.c_ssbo_sphere);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, proxy.cg_vbo);
			//if(proxy.TIMESTEPS > 1)
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, proxy.c_ssbo_weights);
			//else
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, proxy.c_ssbo_traj);

			//uniforms
			glUniform1ui(1, proxy.ATOMCOUNT);
			glUniform1i(2, proxy.TIMESTEPS);
			glUniform1f(3, proxy.t);
			glUniform1ui(4, proxy.SPHEREVERTICES);
			glUniform1f(5, 0.05f);
			glUniform4f(6, 0.75f, 0.5f, 0.4f, 1.f);
			glUniform1f(7, 1.f / proxy.TIMESTEPS);
			glUniform3fv(8, 1, glm::value_ptr(proxy.dims));

			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			glDispatchCompute(proxy.ATOMCOUNT, 1, 1);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, 0);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, 0);

			proxy.compShader.unbind();

			glMemoryBarrier(GL_ALL_BARRIER_BITS);

			// -------------------- Geometry Pass --------------------
			glBindFramebuffer(GL_FRAMEBUFFER, proxy.g_fb);
			//glClearColor(0.5f, 0.5f, 0.5f, 1.f);
			glStencilMask(~0);
			glClearColor(proxy.clearColor[0], proxy.clearColor[1], proxy.clearColor[2], proxy.clearColor[3]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			proxy.geomShader.bind();
			glBindVertexArray(proxy.g_vao);

			glUniform4fv(9, 1, glm::value_ptr(proxy.atomColor));
			glUniformMatrix4fv(10, 1, false, glm::value_ptr(proxy.cam.combined));

			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

			glDrawElements(GL_TRIANGLES, proxy.INDEXCOUNT, GL_UNSIGNED_INT, (void*)0);

			glBindVertexArray(0);
			proxy.geomShader.unbind();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, proxy.g_fb);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBlitFramebuffer(0, 0, proxy.wWidth, proxy.wHeight, 0, 0, proxy.wWidth, proxy.wHeight, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// -------------------- Lightning Pass --------------------
		
			proxy.lightShader.bind();

			glBindVertexArray(proxy.l_vao);

			//bind gbuffer
			glUniform1i(2, 0);
			glUniform1i(3, 1);
			glUniform1i(4, 2);
			glUniform1i(5, 3);
			glUniform1i(6, 4);
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, proxy.g_pos);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, proxy.g_nrm);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, proxy.g_t);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, proxy.g_bt);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, proxy.g_col);

			//uniforms
			glUniformMatrix4fv(7, 1, false, glm::value_ptr(proxy.cam.combined));
			glUniform1fv(10, proxy.lights.size(), proxy.lights.data());
		
			glStencilMask(0x00);
			glStencilFunc(GL_EQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			glDrawElements(GL_TRIANGLES, 6u, GL_UNSIGNED_INT, (void*)0);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);

			glBindVertexArray(0);
			proxy.lightShader.unbind();
					
			
			// -------------------- Forward Pass --------------------
			glViewport(0, 0, proxy.widgetWidth, proxy.widgetHeight);

			glBindVertexArray(proxy.widget_vao);
			proxy.widgetShader.bind();

			Mat4 rot;
			{
				rot[0][0] = proxy.cam.direction.x; rot[0][1] = proxy.cam.direction.y;  rot[0][2] = proxy.cam.direction.z;  rot[0][3] = 0.0f;
				rot[1][0] = proxy.cam.right.x; rot[1][1] = proxy.cam.right.y;  rot[1][2] = proxy.cam.right.z;  rot[1][3] = 0.0f;
				rot[2][0] = proxy.cam.up.x; rot[2][1] = proxy.cam.up.y;  rot[2][2] = proxy.cam.up.z;  rot[2][3] = 0.0f;
				rot[3][0] = 0.f; rot[3][1] = 0.f;  rot[3][2] = 0.f;  rot[3][3] = 1.0f;
			}

			glUniformMatrix4fv(1, 1, false, glm::value_ptr(proxy.widgetCam.combined));
			glUniformMatrix4fv(2, 1, false, glm::value_ptr(rot));

			glDrawElements(GL_TRIANGLES, 420, GL_UNSIGNED_INT, (void*)0);

			proxy.widgetShader.unbind();
			glBindVertexArray(0);
			
			

			// -------------------- SYNC --------------------
			glClientWaitSync(sync[index], GL_SYNC_FLUSH_COMMANDS_BIT, 1000);
			glDeleteSync(sync[index]);
			sync[index] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
			index = (index++) % 2;
		

		// -------------------- FPS --------------------
			frame++;
			if (ctime - time >= 1.0) {
				//LOG(std::to_string(proxy.t) + " " + std::to_string(frame), proxy.logMutex);
				std::cout << proxy.t << " " << frame << std::endl;
				frame = 0;
				time = ctime;
			}
		}

		glfwSwapBuffers(proxy.window);	
		
		proxy.t += proxy.deltaT;
		proxy.t -= std::floor(proxy.t);
		
	}
	async.join();
	glfwTerminate();
	return 0;
}
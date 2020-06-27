
#include "GL.h"

#include "Spline.hpp"

Logger* Logger::instance = new Logger();

void Logger::init() {
	get()->start = std::chrono::high_resolution_clock::now();
};

void Logger::LOG(const std::string& _string, bool _ts) {
	std::lock_guard<std::mutex>(get()->mutex);
	std::chrono::duration<float> elapsed = std::chrono::high_resolution_clock::now() - get()->start;
	float ms = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());
	float s = ms / 1000.f;
	std::cout << (_ts ? " [" + std::to_string(s) + "s]" : "\t") << "\t" << _string << std::endl;
};

Logger* Logger::get() {
	return instance;
};

void ShaderProgram::print(std::string _id, ShaderProgram::Status _compComp, ShaderProgram::Status _compVert,
	ShaderProgram::Status _compGeom, ShaderProgram::Status _compFrag, ShaderProgram::Status _link, std::string _errorLog) {
	if (!printDebug) return;
	Logger::LOG("   Shader: " + std::string(_id), true);
	Logger::LOG("Compiling: "
		+ std::string(_compComp == Status::failed ? " X |" : _compComp == Status::success ? " S |" : " - |")
		+ std::string(_compVert == Status::failed ? " X |" : _compVert == Status::success ? " S |" : " - |")
		+ std::string(_compGeom == Status::failed ? " X |" : _compGeom == Status::success ? " S |" : " - |")
		+ std::string(_compFrag == Status::failed ? " X |" : _compFrag == Status::success ? " S |" : " - |")
		+ "", false);
	Logger::LOG("  Linking: " + std::string(_link == Status::failed ? "Failed!" : _link == Status::success ? "Success!" : " - "), false);

	if (!_errorLog.empty()) {
		Logger::LOG(std::string(_errorLog) + "", false);
	} else
		Logger::LOG("", false);

}

bool ShaderProgram::compileFromFile(const std::string& _path) {
	bool cExists = true;
	bool vExists = true;
	bool gExists = true;
	bool fExists = true;

	std::ifstream compB(_path + ".comp");
	cExists = compB.good();

	std::ifstream vertB(_path + ".vert");
	vExists = vertB.good();

	std::ifstream geomB(_path + ".geom");
	gExists = geomB.good();

	std::ifstream fragB(_path + ".frag");
	fExists = fragB.good();

	bool success = compile(
		(cExists ? std::string{ std::istreambuf_iterator<char>(compB), std::istreambuf_iterator<char>() } : "").c_str(),
		(vExists ? std::string{ std::istreambuf_iterator<char>(vertB), std::istreambuf_iterator<char>() } : "").c_str(),
		(gExists ? std::string{ std::istreambuf_iterator<char>(geomB), std::istreambuf_iterator<char>() } : "").c_str(),
		(fExists ? std::string{ std::istreambuf_iterator<char>(fragB), std::istreambuf_iterator<char>() } : "").c_str());

	compB.close();
	vertB.close();
	geomB.close();
	fragB.close();

	return success;
}

bool ShaderProgram::compile(const char* _compute, const char* _vertex, const char* _geom, const char* _frag) {
	Status compStatus = Status::missing;
	Status vertStatus = Status::missing;
	Status geomStatus = Status::missing;
	Status fragStatus = Status::missing;
	Status linkStatus = Status::missing;

	//std::cout << _compute << std::endl;

	if (compute != -1) {
		glDeleteShader(compute);
		compute = -1;
	}
	if (vertex != -1) {
		glDeleteShader(vertex);
		vertex = -1;
	}
	if (geom != -1) {
		glDeleteShader(geom);
		geom = -1;
	}
	if (frag != -1) {
		glDeleteShader(frag);
		frag = -1;
	}
	if (program != -1) {
		glDeleteShader(program);
		program = -1;
	}

	//Compile Compute
	if (_compute != NULL && _compute[0] != '\0') {
		compute = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute, 1, &_compute, nullptr);
		glCompileShader(compute);
		GLint isCompiled = 0;
		glGetShaderiv(compute, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(compute, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(compute, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(compute);
			compStatus = Status::failed;
			print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
			return false;
		} else compStatus = Status::success;
	}

	//Compile Vertex
	if (_vertex != NULL && _vertex[0] != '\0') {
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &_vertex, nullptr);
		glCompileShader(vertex);
		GLint isCompiled = 0;
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(vertex, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(vertex);
			vertStatus = Status::failed;
			print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
			return false;
		} else vertStatus = Status::success;
	}

	//Compile Geom
	if (_geom != NULL && _geom[0] != '\0') {
		geom = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geom, 1, &_geom, nullptr);
		glCompileShader(geom);
		GLint isCompiled = 0;
		glGetShaderiv(geom, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(geom, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(geom, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(geom);
			geomStatus = Status::failed;
			print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
			return false;
		} else geomStatus = Status::success;
	}

	//Compile Frag
	if (_frag != NULL && _frag[0] != '\0') {
		frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &_frag, nullptr);
		glCompileShader(frag);
		GLint isCompiled = 0;
		glGetShaderiv(frag, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			GLint maxLength = 0;
			glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(frag, maxLength, &maxLength, &errorLog[0]);
			glDeleteShader(frag);
			fragStatus = Status::failed;
			print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
			return false;
		} else fragStatus = Status::success;
	}

	//Link
	program = glCreateProgram();
	if (_compute != NULL && _compute[0] != '\0') glAttachShader(program, compute);
	if (_vertex != NULL && _vertex[0] != '\0') glAttachShader(program, vertex);
	if (_geom != NULL && _geom[0] != '\0') glAttachShader(program, geom);
	if (_frag != NULL && _frag[0] != '\0') glAttachShader(program, frag);

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);
		if (compute != -1)glDeleteShader(compute);
		if (vertex != -1)glDeleteShader(vertex);
		if (geom != -1)glDeleteShader(geom);
		if (frag != -1)glDeleteShader(frag);
		if (program != -1) glDeleteProgram(program);
		linkStatus = Status::failed;

		print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, std::string(errorLog.begin(), errorLog.end()));
		return false;
	} else linkStatus = Status::success;

	if (_compute != NULL && _compute[0] != '\0')glDetachShader(program, compute);
	if (_vertex != NULL && _vertex[0] != '\0')glDetachShader(program, vertex);
	if (_geom != NULL && _geom[0] != '\0')glDetachShader(program, geom);
	if (_frag != NULL && _frag[0] != '\0')glDetachShader(program, frag);

	print(id, compStatus, vertStatus, geomStatus, fragStatus, linkStatus, "");

	unbind();
	return true;
}

GLuint ShaderProgram::getHandle() {
	return program;
}

ShaderProgram::ShaderProgram(std::string _id) : id(_id) {}

ShaderProgram::ShaderProgram() : ShaderProgram(""){}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(program);
}

void ShaderProgram::bind() {
	glUseProgram(getHandle());
}

void ShaderProgram::unbind() {
	glUseProgram(0);
}

Camera::Camera(bool /*_isOrtho*/, const float _viewportWidth, const float _viewportHeight) :viewportWidth(_viewportWidth), viewportHeight(_viewportHeight) {
	direction = UVZ;
	up = UVY;
	normalizeUp();
}

void Camera::update() {
	normalizeUp();

	float aspect = viewportWidth / viewportHeight;
	if(isOrtho)
		projection = glm::ortho(-viewportWidth/2.f, viewportWidth/2.f, -viewportHeight/2.f, viewportHeight/2.f, abs(nearPlane), abs(farPlane));
	else
	projection = glm::perspective(glm::radians(fieldOfView), aspect, std::abs(nearPlane), std::abs(farPlane));
	view = glm::lookAt(position, position + direction, up);
	combined = projection * view;
}

void Camera::lookAt(const Vec3& _pos) {
	direction = glm::normalize(_pos - position);
	normalizeUp();
	view = glm::lookAt(position, _pos, up);
}

void Camera::normalizeUp() {
	right = glm::normalize(glm::cross(direction, up));
	//up = NOR(CRS(right, direction));
}

std::pair<std::vector<float>, std::vector<uint>> Icosahedron::create(uint _subdivisions) {
	
	using IndexedMesh = std::pair<std::vector<float>, std::vector<uint>>;
	const float X = .525731112119133606f;
	const float Z = .850650808352039932f;
	const float N = 0.f;

	struct Triangle { uint vertex[3]; };

	using TriangleList = std::vector<Triangle>;
	using VertexList = std::vector<Vec3>;

	const VertexList vertices =
	{
	  {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z},
	  {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X},
	  {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
	};

	const TriangleList triangles =
	{
	  {0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
	  {8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
	  {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
	  {6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
	};

	using Lookup = std::map<std::pair<uint, uint>, uint>;

	auto vertex_for_edge = [&](Lookup& lookup, VertexList& vertices, uint first, uint second)->uint {
		Lookup::key_type key(first, second);
		if (key.first > key.second)
			std::swap(key.first, key.second);

		auto inserted = lookup.insert({ key, static_cast<uint>(vertices.size()) });
		if (inserted.second) {
			auto& edge0 = vertices[first];
			auto& edge1 = vertices[second];
			auto point = normalize(edge0 + edge1);
			vertices.push_back(point);
		}

		return inserted.first->second;
	};

	auto subdivide = [&](VertexList& vertices, TriangleList triangles)->TriangleList {
		Lookup lookup;
		TriangleList result;

		for (auto&& each : triangles) {
			std::array<uint, 3> mid;
			for (int edge = 0; edge < 3; ++edge) {
				mid[edge] = vertex_for_edge(lookup, vertices,
					each.vertex[edge], each.vertex[(edge + 1) % 3]);
			}

			result.push_back({ each.vertex[0], mid[0], mid[2] });
			result.push_back({ each.vertex[1], mid[1], mid[0] });
			result.push_back({ each.vertex[2], mid[2], mid[1] });
			result.push_back({ mid[0], mid[1], mid[2] });
		}

		return result;
	};

	VertexList vert = vertices;
	TriangleList tria = triangles;

	for (unsigned int i = 0; i < _subdivisions; ++i) {
		tria = subdivide(vert, tria);
	}

	std::vector<float> v_out(vert.size() * 3);
	std::vector<uint> i_out(tria.size() * 3);

	for (size_t i = 0, j = 0; i < vert.size(); ++i, j += 3) {
		auto v = glm::normalize(vert[i]);
		v_out[j] = v[0];
		v_out[j + 1] = v[1];
		v_out[j + 2] = v[2];
	}

	//for (const auto& t : tria) {
	for (size_t i = 0, j = 0; i < tria.size(); ++i, j += 3) {
		auto& t = tria[i];
		i_out[j] = t.vertex[0];
		i_out[j + 1] = t.vertex[1];
		i_out[j + 2] = t.vertex[2];
	}

	return{ v_out, i_out };
}

#if USE_BINARY
void FileParser::loadFile(std::string _path, std::vector<float>& _coords, uint& _count, Vec3& _low, Vec3& _up, Vec3& _dims) {
	Logger::LOG("\t" + _path, false);
	std::ifstream in(_path, std::ios::binary | std::ios::in);
	std::vector<char> buffer(std::istreambuf_iterator<char>(in), {});	
	_low.x = _low.y = _low.z = std::numeric_limits<float>::infinity();
	_up.x = _up.y = _up.z = -std::numeric_limits<float>::infinity();

	_count = static_cast<uint>(*reinterpret_cast<double*>(&buffer[0]));
	_dims[0] = static_cast<float>(*reinterpret_cast<double*>(&buffer[1 * sizeof(double)]));
	_dims[1] = static_cast<float>(*reinterpret_cast<double*>(&buffer[2 * sizeof(double)]));
	_dims[2] = static_cast<float>(*reinterpret_cast<double*>(&buffer[3 * sizeof(double)]));

	const uint c = buffer.size() / sizeof(double) - 4;
	_coords.resize(c + _count*3);

	for (size_t i = 0, j = 4; i < c; i+=3, j+=3) {
		float x = static_cast<float>(*reinterpret_cast<double*>(&buffer[j * sizeof(double)]));
		float y = static_cast<float>(*reinterpret_cast<double*>(&buffer[(j+1) * sizeof(double)]));
		float z = static_cast<float>(*reinterpret_cast<double*>(&buffer[(j+2) * sizeof(double)]));

		_coords[i] = x;
		_coords[i+1] = y;
		_coords[i+2] = z;

		_low.x = x < _low.x ? x : _low.x;
		_low.y = y < _low.y ? y : _low.y;
		_low.z = z < _low.x ? z : _low.z;

		_up.x = x > _up.x ? x : _up.x;
		_up.y = y > _up.y ? y : _up.y;
		_up.z = z > _up.x ? z : _up.z;
	}

	std::memcpy(_coords.data() + c, _coords.data(), _count * 3 * sizeof(float));

}
#else
void FileParser::loadFile(std::string _path, std::vector<float>& _coords, uint& _count, Vec3& _low, Vec3& _up, Vec3& _dims) {
	Logger::LOG("\t" + _path, false);
	std::fstream in(_path);
	char line[256];
	uint i = 0;
	_low.x = _low.y = _low.z = std::numeric_limits<float>::infinity();
	_up.x = _up.y = _up.z = -std::numeric_limits<float>::infinity();
	while (in.getline(line, 256)) {
		if (!i) _count = std::atoi(line);
		else if (i == 1) {
			std::istringstream sstr(line);
			float x, y, z;
			sstr >> x >> y >> z;
			_dims = Vec3(x, y, z);
		} else {
			std::istringstream sstr(line);
			float x, y, z;
			sstr >> x >> y >> z;
			_coords.emplace_back(x);
			_coords.emplace_back(y);
			_coords.emplace_back(z);

			_low.x = x < _low.x ? x : _low.x;
			_low.y = y < _low.y ? y : _low.y;
			_low.z = z < _low.x ? z : _low.z;

			_up.x = x > _up.x ? x : _up.x;
			_up.y = y > _up.y ? y : _up.y;
			_up.z = z > _up.x ? z : _up.z;
		}
		++i;
	}
	for(uint i = 0; i < _count*3; ++i)
		_coords.emplace_back(_coords[i]);
}
#endif // USE_BINARY

CameraController::CameraController(Camera* _cam) : camera(_cam){}

void CameraController::mbCB(int _button, int _action, int /*_mods*/) {
	mbDown = _button == GLFW_MOUSE_BUTTON_RIGHT && _action == GLFW_PRESS;
}

void CameraController::cursorCB(double _xpos, double _ypos) {
	if (std::signbit(oldX) || std::signbit(oldY)) {
		oldX = (float)_xpos;
		oldY = (float)_ypos;
		return;
	}

	if (mbDown) {
		
		float deltaX = -glm::radians(((float)_xpos - oldX) * degreesPerPixel);
		float deltaY = -glm::radians(((float)_ypos - oldY) * degreesPerPixel);

		//std::cout << deltaX << " " << deltaY << std::endl;

		//rotate cam
		Mat4 rot = glm::rotate(glm::identity<Mat4>(), deltaX, camera->up);
		Vec4 res = rot * Vec4(camera->direction, 1.f);
		camera->direction = Vec3(res[0], res[1], res[2]);

		Vec3 tmp = glm::normalize(glm::cross(camera->direction, camera->up));
		//TODO FIX KAMERA GOPFERDAMMI
		//if(std::isnan(tmp[0]) || std::isnan(tmp[1]) || std::isnan(tmp[2]))
		//std::cout << glm::to_string(tmp) << std::endl;
		rot = glm::identity<Mat4>();

		rot = glm::rotate(glm::identity<Mat4>(), deltaY, tmp);
		res = glm::normalize(rot * Vec4(camera->direction, 1.f));
		camera->direction = Vec3(res[0], res[1], res[2]);

		
		camera->update();
	}

	oldX = (float)_xpos;
	oldY = (float)_ypos;
}

void CameraController::update(GLFWwindow* _window, float _delta) {

	if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
		Vec3 delta = glm::normalize(camera->direction) * (_delta * velocity);
		camera->position += delta;
	}
	if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		Vec3 delta = glm::normalize(camera->direction) * (-_delta * velocity);
		camera->position += delta;
	}
	if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		Vec3 delta = glm::normalize(glm::cross(camera->direction, camera->up)) * (-_delta * velocity);
		camera->position += delta;
	}
	if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		Vec3 delta = glm::normalize(glm::cross(camera->direction, camera->up)) * (_delta * velocity);
		camera->position += delta;
	}

	camera->update();

}

#if !COMPUTE_SPLINE_ON_GPU
void SplineBuilder::build(uint _count, uint _steps, const Vec3& dims, std::vector<float>& _traj, std::vector<float>& _out) {
	//cyclic boundary conditions
	{
		_out.resize(12 * _count * _steps);
		const float hx = dims.x;
		const float hy = dims.y;
		const float hz = dims.z;

		const float hx2 = hx / 2.f;
		const float hy2 = hy / 2.f;
		const float hz2 = hz / 2.f;

		const float t = 1.f / float(_steps);
		const float m2 = (2.f * t) / 3.;
		const float m13 = t / 6.f;


		for (uint idx = 0; idx < _count; ++idx) {

			//remove cyclic boundary conditions
			float osx = 0.f, osy = 0.f, osz = 0.f;

			for (uint i = 1; i < _steps; ++i) {
				const uint offsetL = 3 * idx + (i - 1) * _count * 3;
				const uint offsetH = 3 * idx + i * _count * 3;

				const float dx = _traj[offsetH] - _traj[offsetL];
				const float dy = _traj[offsetH + 1] - _traj[offsetL + 1];
				const float dz = _traj[offsetH + 2] - _traj[offsetL + 2];

				_traj[offsetL] += osx * hx;
				_traj[offsetL + 1] += osy * hy;
				_traj[offsetL + 2] += osz * hz;

				osx += dx >= hx2 ? -1.f : dx <= -hx2 ? 1.f : 0.f;
				osy += dy >= hy2 ? -1.f : dy <= -hy2 ? 1.f : 0.f;
				osz += dz >= hz2 ? -1.f : dz <= -hz2 ? 1.f : 0.f;
			}

			std::vector<float> tmp(_steps);
			std::vector<float> x(_steps);

			//dims
			for (uint k = 0; k < 3; ++k) {

				//rhs
				for (uint i = 1; i < _steps - 1; ++i) {

					const uint offsetL = 3 * idx + (i - 1) * _count * 3 + k;
					const uint offsetM = 3 * idx + i * _count * 3 + k;
					const uint offsetH = 3 * idx + (i + 1) * _count * 3 + k;

					x[i] = (_traj[offsetH] - _traj[offsetM]) / t - (_traj[offsetM] - _traj[offsetL]) / t;
					tmp[i] = 0.f;
				}

				//boundary conditions
				x[0] = 3.f * ((_traj[3 * idx + _count * 3 + k] - _traj[3 * idx + k]) / t);
				x[_steps - 1] = 3.0 * (-(_traj[3 * idx + (_steps - 1) * _count * 3 + k] - _traj[3 * idx + (_steps - 2) * _count * 3 + k]) / t);

				//solve
				tmp[0] = m13 / m2;
				x[0] = x[0] / m2;

				for (uint i = 1; i < _steps; ++i) {
					const float m = 1.f / (m2 - m13 * tmp[i - 1]);
					tmp[i] = m13 * m;
					x[i] = (x[i] - m13 * x[i - 1]) * m;
				}

				for (uint i = _steps - 2; i > 0; --i)
					x[i] -= tmp[i] * x[i + 1];
				x[0] -= tmp[0] * x[1];

				if (idx == 0) {
					for (uint i = 0; i < _steps; ++i)
						_out[i] = x[i];
				}

				//calc weights
				for (uint i = 1; i < _steps; ++i) {

					const uint offsetL = 3 * idx + (i - 1) * _count * 3 + k;
					const uint offsetM = 3 * idx + i * _count * 3 + k;

					const uint offset = 12 * idx + (i - 1) * _count * 12 + k;

					_out[offset] = _traj[offsetL];
					_out[offset + 3] = (_traj[offsetM] - _traj[offsetL]) / t - (t * (2 * x[i - 1] + x[i])) / 6.f;
					_out[offset + 6] = x[i - 1] / 2.f;
					_out[offset + 9] = (x[i] - x[i - 1]) / (6 * t);

				}

			}
		}
		
	}

}
#endif // COMPUTE_SPLINE_ON_GPU



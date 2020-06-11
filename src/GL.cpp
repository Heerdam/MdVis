
#include "GL.h"

void ShaderProgram::print(std::string _id, ShaderProgram::Status _compComp, ShaderProgram::Status _compVert,
	ShaderProgram::Status _compGeom, ShaderProgram::Status _compFrag, ShaderProgram::Status _link, std::string _errorLog) {
	if (!printDebug) return;
	LOG("   Shader: " + std::string(_id));
	LOG("Compiling: "
		+ std::string(_compComp == Status::failed ? " X |" : _compComp == Status::success ? " S |" : " - |")
		+ std::string(_compVert == Status::failed ? " X |" : _compVert == Status::success ? " S |" : " - |")
		+ std::string(_compGeom == Status::failed ? " X |" : _compGeom == Status::success ? " S |" : " - |")
		+ std::string(_compFrag == Status::failed ? " X |" : _compFrag == Status::success ? " S |" : " - |")
		+ "\n");
	LOG("  Linking: " + std::string(_link == Status::failed ? "Failed!" : _link == Status::success ? "Success!" : " - ") + "\n");

	if (_errorLog.empty()) {
		LOG("\n");
	} else {
		LOG("\n" + std::string(_errorLog) + "\n");
	}

	//GLError(_id);
	LOG("\n");
}

bool ShaderProgram::compileFromFile(const std::string& _path) {
	bool cExists = true;
	bool vExists = true;
	bool gExists = true;
	bool fExists = true;

	std::ifstream comp(_path + ".comp");
	cExists = comp.good();

	std::ifstream vert(_path + ".vert");
	vExists = vert.good();

	std::ifstream geom(_path + ".geom");
	gExists = geom.good();

	std::ifstream frag(_path + ".frag");
	fExists = frag.good();

	bool success = compile(
		(cExists ? std::string{ std::istreambuf_iterator<char>(comp), std::istreambuf_iterator<char>() } : "").c_str(),
		(vExists ? std::string{ std::istreambuf_iterator<char>(vert), std::istreambuf_iterator<char>() } : "").c_str(),
		(gExists ? std::string{ std::istreambuf_iterator<char>(geom), std::istreambuf_iterator<char>() } : "").c_str(),
		(fExists ? std::string{ std::istreambuf_iterator<char>(frag), std::istreambuf_iterator<char>() } : "").c_str());

	comp.close();
	vert.close();
	geom.close();
	frag.close();

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

Camera::Camera(bool _isOrtho, const float _viewportWidth, const float _viewportHeight) :viewportWidth(_viewportWidth), viewportHeight(_viewportHeight) {
	direction = UVZ;
	up = UVY;
	normalizeUp();
}

void Camera::update() {
	normalizeUp();

	float aspect = viewportWidth / viewportHeight;
	if(isOrtho)
		projection = ORTHO(-viewportWidth/2.f, viewportWidth/2.f, -viewportHeight/2.f, viewportHeight/2.f, aspect, ABS(nearPlane), ABS(farPlane));
	else
		projection = PERSPECTIVE(TORAD(fieldOfView), aspect, ABS(nearPlane), ABS(farPlane));
	view = LOOKAT(position, position + direction, up);
	combined = projection * view;
}

void Camera::lookAt(const Vec3& _pos) {
	direction = NOR(_pos - position);
	normalizeUp();
	view = LOOKAT(position, _pos, up);
}

void Camera::normalizeUp() {
	right = NOR(CRS(direction, up));
	up = NOR(CRS(right, direction));
}

void Camera::normalizeUpAxisLocked(const Vec3& _up) {
	right = NOR(CRS(_up, direction));
	up = NOR(CRS(direction, right));
	if (std::isnan(up[0]) || std::isnan(up[1]) || std::isnan(up[2]))
		up = _up;
}

void Camera::arcball() {
	float x = distance * SIN(TORAD(height)) * COS(TORAD(azimuth)) + target[0];
	float y = distance * SIN(TORAD(height)) * SIN(TORAD(azimuth)) + target[1];
	float z = distance * COS(TORAD(height)) + target[2];

	position.x = x;
	position.y = z;
	position.z = y;
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

float volatile FileParser::progress = 0.f;

void FileParser::parse(std::string _path, std::vector<float>& _coords, uint& _count) {
	std::fstream in(_path);
	char line[256];
	uint i = 0;
	while (in.getline(line, 256)) {
		if (!i) _count = std::atoi(line);
		else {
			std::istringstream sstr(line);
			float x, y, z;
			sstr >> x >> y >> z;
			_coords.emplace_back(x * 100.f);
			_coords.emplace_back(y * 100.f);
			_coords.emplace_back(z * 100.f);
		}
		++i;
	}
}

CameraController::CameraController(Camera* _cam) : camera(_cam){}

void CameraController::mbCB(int _button, int _action, int _mods) {
	mbDown = _button == GLFW_MOUSE_BUTTON_RIGHT && _action == GLFW_PRESS;
}

void CameraController::cursorCB(double _xpos, double _ypos) {
	if (std::signbit(oldX) || std::signbit(oldY)) {
		oldX = (float)_xpos;
		oldY = (float)_ypos;
		return;
	}

	if (mbDown) {
		

		float deltaX = -TORAD(((float)_xpos - oldX) * degreesPerPixel);
		float deltaY = -TORAD(((float)_ypos - oldY) * degreesPerPixel);

		//std::cout << deltaX << " " << deltaY << std::endl;

		//rotate cam
		Mat4 rot = glm::rotate(glm::identity<Mat4>(), deltaX, camera->up);;

		Vec4 res = rot * Vec4(camera->direction, 1.f);
		camera->direction = Vec3(res[0], res[1], res[2]);

		Vec3 tmp = NOR(CRS(camera->direction, camera->up));

		rot = glm::identity<Mat4>();

		rot = glm::rotate(glm::identity<Mat4>(), deltaY, tmp);
		res = rot * Vec4(camera->direction, 1.f);
		camera->direction = Vec3(res[0], res[1], res[2]);

		camera->update();
	}

	oldX = (float)_xpos;
	oldY = (float)_ypos;
}

void CameraController::update(GLFWwindow* _window, float _delta) {

	if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
		Vec3 delta = NOR(camera->direction) * (_delta * velocity);
		camera->position += delta;
	}
	if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		Vec3 delta = NOR(camera->direction) * (-_delta * velocity);
		camera->position += delta;
	}
	if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		Vec3 delta = NOR(CRS(camera->direction, camera->up)) * (-_delta * velocity);
		camera->position += delta;
	}
	if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		Vec3 delta = NOR(CRS(camera->direction, camera->up)) * (_delta * velocity);
		camera->position += delta;
	}

	camera->update();

}


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
	//GLError("ShaderProgram::bind");
}

void ShaderProgram::unbind() {
	glUseProgram(0);
	//GLError("ShaderProgram::unbind");
}

SSBO::SSBO(std::string _id, uint _size, void* _data, GLbitfield _flags) : id(_id), flags(_flags) {
	glGenBuffers(1, &handle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, _size, _data, flags);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//GLError("SSBO::SSBO" + id);
}

SSBO::~SSBO() {
	unmap();
	glDeleteBuffers(1, &handle);
	//GLError("SSBO::~SSBO" + id);
}

void SSBO::bind(uint _binding) {
	bindAs(GL_SHADER_STORAGE_BUFFER, _binding);
}

void SSBO::bindAs(uint _target, uint _binding) {
	glBindBufferBase(lastBindTarget = _target, _binding, handle);
	//GLError("SSBO::bindAs::" + id);
}

void SSBO::unbind() {
	glBindBuffer(lastBindTarget, 0);
	//GLError("SSBO::unbind::" + id);
}

void* SSBO::map(uint _offset, uint _length, GLbitfield _flags) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	pntr = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, _offset, _length, _flags);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//GLError("SSBO::map::" + id);
	return pntr;
}

void SSBO::unmap() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, handle);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//GLError("SSBO::unmap::" + id);
}

VBO::VBO(std::string _id, GLsizeiptr _size, const GLvoid* _data, GLenum _usage, uint _vertexCount) : id(_id), size(_vertexCount) {
	glGenBuffers(1, &handle);
	bind();
	glBufferData(GL_ARRAY_BUFFER, _size, _data, _usage);
	unbind();
	//GLError("VBO::VBO::" + id);
}

VBO::~VBO() {
	glDeleteBuffers(1, &handle);
	//GLError("VBO::~VBO::" + id);
}

void VBO::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, handle);
	//GLError("VBO::bind::" + id);
}

void VBO::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//GLError("VBO::unbind::" + id);
}

void VBO::bindAs(uint _target, uint _binding) {
	glBindBufferBase(_target, _binding, handle);
	//GLError("VBO::bindAs::" + id);
}

void VBO::enableVertexAttribute(GLuint _index, GLint _size, GLenum _type, GLboolean _normalized, GLsizei _stride, const GLvoid* _pointer) {
	glVertexAttribPointer(_index, _size, _type, _normalized, _stride, _pointer);
	glEnableVertexAttribArray(_index);
	//GLError("VBO::enableVertexAttribute::" + id);
}

void VBO::drawAll(GLenum _mode) {
	draw(_mode, 0, size);
}

void VBO::draw(GLenum _mode, GLint _first, GLsizei _count) {
	glDrawArrays(_mode, _first, _count);
	//GLError("VBO::draw::" + id);
}

EBO::EBO(std::string _id, GLsizeiptr _size, const GLvoid* _data, GLenum _usage, uint _indexCount) : id(_id), size(_indexCount) {
	glGenBuffers(1, &handle);
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _size, _data, _usage);
	unbind();
	//GLError("EBO::EBO::" + id);
}

EBO::~EBO() {
	glDeleteBuffers(1, &handle);
	//GLError("EBO::~EBO::" + id);
}

void EBO::bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
	//GLError("EBO::bind::" + id);
}

void EBO::unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
	//GLError("EBO::unbind::" + id);
}

void EBO::drawAll(GLenum _mode) {
	draw(_mode, 0, size, (void*)0x0);
}

void EBO::draw(GLenum _mode, GLint _count, GLenum _type, const GLvoid* _offset) {
	glDrawElements(_mode, _count, _type, _offset);
	//GLError("EBO::draw::" + id);
}

Camera::Camera(const float _viewportWidth, const float _viewportHeight) :viewportWidth(_viewportWidth), viewportHeight(_viewportHeight) {
	direction = Vec4(0.f, 0.f, -1.f, 1.f);
	up = UVY;
}

void Camera::update() {
	direction = NOR(Vec4(target[0], target[1], target[2], 1.f) - position);
	normalizeUpAxisLocked(UVY);

	float aspect = viewportWidth / viewportHeight;
	projection = PERSPECTIVE(TORAD(fieldOfView), aspect, ABS(nearPlane), ABS(farPlane));
	view = LOOKAT(Vec3(position), Vec3(position + direction), Vec3(up));
	combined = projection * view;
}

void Camera::lookAt(const Vec3& _pos) {
	direction = NOR(Vec4(_pos[0], _pos[1], _pos[2], 1.f) - position);
	normalizeUp();
	view = LOOKAT(Vec3(position), _pos, Vec3(up));
}

void Camera::normalizeUp() {
	right = Vec4(NOR(CRS(Vec3(direction), Vec3(up))), 1.f);
	up = Vec4(NOR(CRS(Vec3(right), Vec3(direction))), 1.f);
}

void Camera::normalizeUpAxisLocked(const Vec3& _up) {
	right = Vec4(NOR(CRS(_up, Vec3(direction))), 1.f);
	up = Vec4(NOR(CRS(Vec3(direction), Vec3(right))), 1.f);
}

void Camera::arcball(const Vec3& _point, const float _azimuth, const float _altitude, const float _radius) {
	float x = _radius * SIN(TORAD(_altitude)) * COS(TORAD(_azimuth)) + _point[0];
	float y = _radius * SIN(TORAD(_altitude)) * SIN(TORAD(_azimuth)) + _point[1];
	float z = _radius * COS(TORAD(_altitude)) + _point[2];

	position.x = x;
	position.y = z;
	position.z = y;
}

void Camera::translate(const Vec3& _delta) {
	position += Vec4(_delta[0], _delta[1], _delta[2], 0.f);
}

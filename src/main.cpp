
#include "GL.h"

#include <GLFW/glfw3.h>

#include <sstream>
#include <map>
#include <functional>
#include <array>

void parseFile(std::string _path, std::vector<float>& _coords, uint& _count) {
	std::fstream in(_path);
	char line[256];
	uint i = 0;
	while (in.getline(line, 256)){
		if (!i) _count = std::atoi(line);
		else {
			std::istringstream sstr(line);
			float x, y, z;
			sstr >> x >> y >> z;
			_coords.emplace_back(x);
			_coords.emplace_back(y);
			_coords.emplace_back(z);
		}
	}
};

using IndexedMesh = std::pair<std::vector<float>, std::vector<uint>>;

IndexedMesh icosahedron(uint _subdivisions) {
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

	for (const auto& v : vertices) {
		v_out.emplace_back(v[0]);
		v_out.emplace_back(v[1]);
		v_out.emplace_back(v[2]);
	}

	for (const auto& t : tria) {
		i_out.emplace_back(t.vertex[0]);
		i_out.emplace_back(t.vertex[1]);
		i_out.emplace_back(t.vertex[2]);
	}

	return{ v_out, i_out };

};

int main() {

	if (!glfwInit()) {
		std::cout << "GLFW not ok" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_REFRESH_RATE, 60);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "MdVis", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSwapInterval(0);
	glfwSwapBuffers(window);

	glewExperimental = GL_TRUE;
	auto status = glewInit();
	if (!status == GLEW_OK) {
		std::cout << "glew not ok" << std::endl;
		return -1;
	}

	//parse file
	std::string path = "out.traj";
	uint size; //atom count
	std::vector<float> coords;
	//parseFile(path, coords, size);

	coords.emplace_back(0.f);
	coords.emplace_back(0.f);
	coords.emplace_back(0.f);
	size = 1;

	const uint timesteps = static_cast<uint>(coords.size()) / size;
	auto sphere = icosahedron(3);

	//load shaders
	ShaderProgram compShader("compute Shader");
	compShader.compile("compute Shader", "../shader/time1");
	ShaderProgram shader("Shader");
	compShader.compile("Shader", "../shader/");

	//create buffers
	SSBO ssbo_traj("ssbo_traj", size * sizeof(float), coords.data(), GL_STATIC_DRAW);
	SSBO ssbo_sphere("ssbo_sphere", static_cast<uint>(std::get<0>(sphere).size()) * sizeof(float), std::get<0>(sphere).data(), GL_STATIC_DRAW);
	
	VBO vbo("vbo", 9 * size * timesteps * sizeof(float), nullptr, GL_STATIC_DRAW, 9 * size * timesteps);

	std::vector<uint> indices(std::get<1>(sphere).size() * size);
	auto& sphere_indices = std::get<1>(sphere);
	for (uint s = 0; s < size; ++s)
		for (uint i = 0; i < sphere_indices.size(); ++i)
			indices[s * size + i] = sphere_indices[i] + s * size;

	EBO ebo("ebo", indices.size() * sizeof(uint), indices.data(), 0, size);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	ebo.bind();

	vbo.bind();
	//pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//nrm
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	//col
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	Camera cam(1920, 1080);

	float currentT = 0.f;
	while (!glfwWindowShouldClose(window)) {

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwPollEvents();

		cam.update();

		//create vertexbuffer
		compShader.bind();
		//buffers
		ssbo_traj.bind(1);
		ssbo_sphere.bind(2);
		vbo.bindAs(GL_SHADER_STORAGE_BUFFER, 3);
		//uniforms
		glUniform1ui(1, size);
		glUniform1ui(2, timesteps);
		glUniform1f(3, currentT);
		glUniform1ui(4, static_cast<uint>(std::get<0>(sphere).size()) / 3);
		glUniform1f(5, 25.f);
		const float col[4] = { 0.75f, 0.5f, 0.5f, 1.f };
		glUniform4fv(6, 4, col);

		glDispatchCompute(size, 1, 0);

		compShader.unbind();

		//draw
		shader.bind();
		glUniformMatrix4fv(3, 16, false, ToArray(cam.combined));
		glBindVertexArray(VAO);
		ebo.drawAll(GL_TRIANGLES);
		glBindVertexArray(0);
		shader.unbind();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
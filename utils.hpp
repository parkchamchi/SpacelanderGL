#ifndef UTILS_HPP
#define UTILS_HPP

#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>

#include <string>

using namespace std;

const float PI = 3.1415f;

void draw_circle(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius);
void draw_line(glm::mat4 projection, glm::mat4 view, glm::vec3 p, glm::vec3 q);
void utils_cleanup();

class Circle {
public:
	Circle();
	~Circle();

	void draw(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius);

private:
	const int VLEN = 360; //# of vertices
	float *vertices;
	unsigned int VAO, VBO;
	Shader *shader;
};

class Line {
public:
	Line();
	~Line();

	void draw(glm::mat4 projection, glm::mat4 view, glm::vec3 p, glm::vec3 q);

private:
	const int VLEN = 360;
	float *vertices;
	unsigned int VAO, VBO;
	Shader *shader;
};

#endif
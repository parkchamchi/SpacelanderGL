#include "utils.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <learnopengl/shader_m.h>

#include <cmath>
#include <iostream>

using namespace std;

class Shape {
public:
	Shape(int vlen, GLenum mode);
	~Shape();

	virtual void draw(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float scale, glm::vec4 color);

protected:
	int get_vlen(); //# of vertices
	float *get_vertices(); //float[vlen * 3]
	void apply_vertices(); //Should be called after changing `vertices`

private:
	GLenum mode;
	int vlen;

	float *vertices;
	unsigned int VAO, VBO;
	Shader *shader;
};

class Circle : public Shape {
public:
	Circle();

	void draw(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius, glm::vec4 color);
};

class Line : public Shape {
public:
	Line();

	void draw(glm::mat4 projection, glm::mat4 view, glm::vec3 p, glm::vec3 q, glm::vec4 color);
};

class Cube : public Shape {
public:
	Cube();

	//void draw(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float size, glm::vec4 color);
};

static Circle *circle = nullptr;
static Line *line = nullptr;
static Cube *cube = nullptr;

void draw_circle(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius, glm::vec4 color) {
	if (circle == nullptr)
		circle = new Circle();

	circle->draw(projection, view, location, radius, color);
}

void draw_line(glm::mat4 projection, glm::mat4 view, glm::vec3 p, glm::vec3 q, glm::vec4 color) {
	if (line == nullptr)
		line = new Line();

	line->draw(projection, view, p, q, color);
}

void draw_cube(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float size, glm::vec4 color) {
	if (cube == nullptr)
		cube = new Cube();

	cube->draw(projection, view, location, size, color);
}

void utils_cleanup() {
	if (circle != nullptr)
		delete circle;
	if (line != nullptr)
		delete line;
	if (cube != nullptr)
		delete cube;
}

Shape::Shape(int vlen, GLenum mode) : vlen(vlen), mode(mode) {
	//glBindVertexArray(0); //Unbind
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Set VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, 3 * vlen * sizeof (float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void *) 0);
	glEnableVertexAttribArray(0);

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); //Unbind

	//Set shader
	shader = new Shader("shaders/monocolor.vs", "shaders/monocolor.fs");

	//Set vertices
	vertices = new float[vlen*3];
}

int Shape::get_vlen() {
	return vlen;
}

float *Shape::get_vertices() {
	return vertices;
}

void Shape::apply_vertices() {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vlen * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
}

void Shape::draw(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float scale, glm::vec4 color) {
	shader->use();
	shader->setVec4("aColor", color);

	shader->setMat4("projection", projection);
	shader->setMat4("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, location);
	model = glm::scale(model, glm::vec3(scale));
	shader->setMat4("model", model);

	glBindVertexArray(VAO);
	glDrawArrays(mode, 0, vlen);	
}

Shape::~Shape() {
	delete shader;
	delete[] vertices;
	glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

Circle::Circle() : Shape(360, GL_LINE_LOOP) {
	int vlen = get_vlen();

	//Set `vertices`
	float *vertices = get_vertices();
	for (int i = 0; i < vlen; i++) {
		float angle = ((float) i / vlen) * 2*PI;
		vertices[i*3 + 0] = cos(angle);
		vertices[i*3 + 1] = 0.0f;
		vertices[i*3 + 2] = sin(angle);
	}
	apply_vertices();
}

void Circle::draw(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius, glm::vec4 color) {
	Shape::draw(projection, view, location, radius, color);
}

Line::Line() : Shape(2, GL_LINES) {
	int vlen = get_vlen();
	float *vertices = get_vertices();
	for (int i = 0; i < vlen*3; i++)
		vertices[i] = 0;
	apply_vertices();
}

void Line::draw(glm::mat4 projection, glm::mat4 view, glm::vec3 p, glm::vec3 q, glm::vec4 color) {
	float *vertices = get_vertices();
	vertices[0] = p.x;
	vertices[1] = p.y;
	vertices[2] = p.z;
	vertices[3] = q.x;
	vertices[4] = q.y;
	vertices[5] = q.z;
	apply_vertices();

	Shape::draw(projection, view, glm::vec3(0), 1, color);
}

Cube::Cube() : Shape(36, GL_TRIANGLES) {
	int vlen = get_vlen();
	float *vertices = get_vertices();
	
	float new_vertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
	memcpy(vertices, new_vertices, sizeof (new_vertices));
	apply_vertices();
}
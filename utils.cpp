#include "utils.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <cmath>
#include <iostream>

using namespace std;

static Circle *circle = nullptr;
static Line *line = nullptr;

void draw_circle(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius) {
	if (circle == nullptr)
		circle = new Circle();

	circle->draw(projection, view, location, radius);
}

void draw_line(glm::mat4 projection, glm::mat4 view, glm::vec3 p, glm::vec3 q) {
	if (line == nullptr)
		line = new Line();

	line->draw(projection, view, p, q);
}

void utils_cleanup() {
	if (circle != nullptr)
		delete circle;
	if (line != nullptr)
		delete line;
}

Circle::Circle() {
	//Set `vertices`
	vertices = new float[VLEN*3];
	for (int i = 0; i < VLEN; i++) {
		float angle = ((float) i / VLEN) * 2*PI;
		vertices[i*3 + 0] = cos(angle);
		vertices[i*3 + 1] = 0.0f;
		vertices[i*3 + 2] = sin(angle);
	}

	//glBindVertexArray(0); //Unbind
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Set VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenVertexArrays(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * VLEN * sizeof (float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void *) 0);
	glEnableVertexAttribArray(0);

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); //Unbind

	//Set shader
	shader = new Shader("shaders/monocolor.vs", "shaders/monocolor.fs");
}

void Circle::draw(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius) {
	shader->use();
	shader->setVec4("aColor", glm::vec4(0, 1, 0, 1));

	shader->setMat4("projection", projection);
	shader->setMat4("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, location);
	model = glm::scale(model, glm::vec3(radius));
	shader->setMat4("model", model);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_LOOP, 0, VLEN);
}

Circle::~Circle() {
	delete shader;
	delete vertices;
	glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

Line::Line() {
	vertices = new float[VLEN * 3] {
		0, 0, 0,
		0, 0, 0,
	};

	//Set VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenVertexArrays(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * VLEN * sizeof (float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void *) 0);
	glEnableVertexAttribArray(0);

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); //Unbind

	//Set shader
	shader = new Shader("shaders/monocolor.vs", "shaders/monocolor.fs");
}

void Line::draw(glm::mat4 projection, glm::mat4 view, glm::vec3 p, glm::vec3 q) {
	shader->use();
	shader->setVec4("aColor", glm::vec4(0, 1, 0, 1));

	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	glm::mat4 model = glm::mat4(1.0f);
	shader->setMat4("model", model);

	vertices[0] = p.x;
	vertices[1] = p.y;
	vertices[2] = p.z;
	vertices[3] = q.x;
	vertices[4] = q.y;
	vertices[5] = q.z;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, VLEN * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, VLEN);
}

Line::~Line() {
	delete shader;
	delete vertices;
	glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
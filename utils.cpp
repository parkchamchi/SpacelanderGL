#include "utils.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <iostream>

using namespace std;

string to_string(glm::vec3 vec) {
	return "vec3<" + to_string(vec.x) + ", " + to_string(vec.y) + ", " + to_string(vec.z) + ">";
}

void draw_circle(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius) {
	static Circle *circle = nullptr;
	if (circle == nullptr)
		circle = new Circle();

	circle->draw(projection, view, location, radius);
}

Circle::Circle() {
	//Set `vertices`
	vertices = new float[VLEN*3];
	for (int i = 0; i < VLEN; i++) {
		float angle = ((float) i / VLEN) * cos(2*PI);
		vertices[i*3 + 0] = cos(angle);
		vertices[i*3 + 1] = 0.0f;
		vertices[i*3 + 2] = sin(angle);
	}


	glBindVertexArray(0); //Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Set VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenVertexArrays(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * VLEN * sizeof (float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void *) 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); //Unbind

	//Set shader
	shader = new Shader("shaders/monocolor.vs", "shaders/monocolor.fs");
}

void Circle::draw(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius) {
	shader->use();
	shader->setVec4("aColor", GREEN_VEC4);

	shader->setMat4("projection", projection);
	shader->setMat4("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, location);
	model = glm::scale(model, glm::vec3(radius * 2));
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
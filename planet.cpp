#include "planet.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <chrono>
#include <iostream>

static float get_time() {
	return (float) glfwGetTime();
}

Planet::Planet(
	Drawable *drawable, Shader *shader,
	float radius,
	float orbit_freq, float orbit_radius,
	float rot_freq, glm::vec3 rot_axis
) 
	: drawable(drawable), shader(shader),
	radius(radius),
	orbit_freq(orbit_freq), orbit_radius(orbit_radius),
	rot_freq(rot_freq), rot_axis(rot_axis)
{
	position = glm::vec3(radius, 0.0f, 0.0f);
	model = glm::mat4(1.0f);
}

Planet::~Planet() {

}

void Planet::update() {
	float time = get_time();

	model = glm::mat4(1.0f);

	//Translate (orbit)
	float x = orbit_radius * std::cos(time * orbit_freq);
	float z = orbit_radius * std::sin(time * orbit_freq);
	position = glm::vec3(x, 0.0f, z);
	model = glm::translate(model, position);
	//std::cout << "Planet loc: " << x << ", " << z << std::endl;	

	glm::vec3 rot_axis(0.0f, 1.0f, 0.0f);
	const float PI = 3.1415f;
	float rot_angle = std::fmod(time, (2 * PI)); //radians
	model = glm::rotate(model, rot_angle, rot_axis);
	model = glm::scale(model, glm::vec3(radius));
}

void Planet::draw(glm::mat4 projection, glm::mat4 view) {
	shader->use();

	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	
	shader->setMat4("model", model);

	drawable->draw(shader);
}
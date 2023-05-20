#include "planet.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <cmath>
#include <iostream>

#include "utils.hpp"

static float get_time() {
	return (float) glfwGetTime();
}

Planet::Planet(
	Drawable *drawable, Shader *shader,
	float radius,
	float orbit_freq, float orbit_radius,
	float rot_freq, glm::vec3 rot_axis,
	ExtraShaderOpT extra_shader_op
) 
	: drawable(drawable), shader(shader),
	radius(radius),
	orbit_freq(orbit_freq), orbit_radius(orbit_radius),
	rot_freq(rot_freq), rot_axis(rot_axis),
	extra_shader_op(extra_shader_op)
{
	position = glm::vec3(radius, 0.0f, 0.0f);
	model = glm::mat4(1.0f);
}

void Planet::update() {
	float time = get_time();

	model = glm::mat4(1.0f);

	//Translate (orbit)
	float x = orbit_radius * std::cos(time * orbit_freq * 2*PI);
	float z = orbit_radius * std::sin(time * orbit_freq * 2*PI);
	position = glm::vec3(x, 0.0f, z);
	model = glm::translate(model, position);
	//std::cout << "Planet loc: " << x << ", " << z << std::endl;	

	glm::vec3 rot_axis(0.0f, 1.0f, 0.0f);
	float rot_angle = time * rot_freq * 2*PI; //radians
	model = glm::rotate(model, rot_angle, rot_axis);
	model = glm::scale(model, glm::vec3(radius));
}

void Planet::draw(glm::mat4 projection, glm::mat4 view) {
	shader->use();

	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	shader->setMat4("model", model);

	if (extra_shader_op != nullptr)
		extra_shader_op(shader, this);

	drawable->draw(shader);

	draw_orbit(projection, view);
}

float Planet::get_radius() {
	return radius;
}

glm::vec3 Planet::get_position() {
	return position;
}

glm::vec3 Planet::get_gravity(glm::vec3 target, float delta_time) {
	//target TO this->position
	glm::vec3 diff = position - target;

	float dist = glm::length(diff);
	glm::vec3 dir = glm::normalize(diff);

	/*
	G * Mm / dist^2 = G'*m*rho*rad^3 / dist^2
	Assuming m never changes,
		K * rho * rad^3 / dist^2
	*/

	const float KRHO = 0.005f;
	float mag = KRHO * std::pow(radius, 3) / std::pow(dist, 2);
	return mag * delta_time * dir;
}

void Planet::draw_orbit(glm::mat4 projection, glm::mat4 view) {
	draw_circle(projection, view, glm::vec3(0), orbit_radius); //orbit
	draw_circle(projection, view, position, radius * 1.1f); //to the planet

	draw_line(projection, view, glm::vec3(0), position); //to the sun;

	draw_line(projection, view, glm::vec3(0), glm::vec3(0, 3, 0)); //To world up
	draw_cube(projection, view, glm::vec3(0)); //origin
}
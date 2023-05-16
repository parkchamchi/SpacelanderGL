#ifndef PLANET_HPP
#define PLANET_HPP

#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>

class Planet;
using ExtraShaderOpT = void (*)(Shader *, Planet *);

class Drawable {
public:	
	virtual void draw(Shader *shader) = 0;
};

class Planet {
public:
	Planet(
		Drawable *drawable, Shader *shader,
		float radius,
		float orbit_freq, float orbit_radius,
		float rot_freq, glm::vec3 rot_axis,
		ExtraShaderOpT extra_shader_op=nullptr
	);

	void update();
	void draw(glm::mat4 projection, glm::mat4 view);

	float get_radius();
	glm::vec3 get_position();

	glm::vec3 get_gravity(glm::vec3 target);

private:
	void draw_orbit(glm::mat4 projection, glm::mat4 view);

	Drawable *drawable;
	Shader *shader;

	float radius;
	float orbit_freq;
	float orbit_radius;
	float rot_freq;
	glm::vec3 rot_axis;

	glm::vec3 position;
	glm::mat4 model;

	ExtraShaderOpT extra_shader_op;
};

#endif
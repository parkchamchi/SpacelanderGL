#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>

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
		float rot_freq, glm::vec3 rot_axis
	);
	~Planet();

	void draw(glm::mat4 projection, glm::mat4 view);

private:
	Drawable *drawable;
	Shader *shader;

	float radius;
	float orbit_freq;
	float orbit_radius;
	float rot_freq;
	glm::vec3 rot_axis;
};
#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>

#include <vector>

class GameObj {
public:
	virtual void draw(glm::mat4 proj, glm::mat4 view) = 0;
};

class Sphere : public GameObj {
public:
	Sphere(glm::vec3 loc, float radius, Shader *shader);
	~Sphere();
	
	virtual void draw(glm::mat4 proj, glm::mat4 view) override;

private:
	unsigned int VBO, VAO;

protected:
	float radius;
	glm::vec3 location;
	Shader* shader;

	float rotation = 0;
	glm::vec3 rotation_direction = glm::vec3(0.0f, 1.0f, 0.0f);
};

class Planet : public Sphere {
public:
	Planet(float distance, float radius, float orbit_speed, float rotation_speed, Shader *shader);

	void update(float delta_time);

	float get_radius();
	float get_distance();

private:
	float distance;
	float orbit_speed;
	float rotation_speed;

	float orbit_angle;
};

class SolarSystem {
public:
	SolarSystem(Shader *star_shader, Shader *planet_shader);
	~SolarSystem();

	void add(float distance_offset, float radius, float orbit_speed, float ration_speed, bool is_star=false);
	void draw(glm::mat4 proj, glm::mat4 view);

private:
	Shader *star_shader, *planet_shader;

	std::vector<Planet *> planets;
};
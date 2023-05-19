#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/glm.hpp>

class Player {
public:
	Player(glm::vec3 position, float front, float yaw);

	void process_input(float forward_offset, float pitch_offset, float yaw_offset);
	void add_gravity(glm::vec3 gravity);

	glm::vec3 get_position();
	void set_position(glm::vec3 position); //init.
	glm::vec3 get_velocity();
	void get_camera_vecs(glm::vec3 *front, glm::vec3 *right, glm::vec3 *up);

	void draw_lines(glm::mat4 projection, glm::mat4 view, glm::vec3 planet_pos);

private:
	glm::vec3 get_front();

	glm::vec3 position;
	float pitch;
	float yaw;

	glm::vec3 velocity;
	glm::vec3 last_gravity;
};

#endif
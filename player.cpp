#include "player.hpp"
#include "utils.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

Player::Player(glm::vec3 position, float pitch, float yaw) : position(position), pitch(pitch), yaw(yaw) {
	velocity = glm::vec3(0.0f);
	last_gravity = glm::vec3(0.0f);
}

void Player::process_input(float forward_offset, float pitch_offset, float yaw_offset) {
	pitch += pitch_offset;
	yaw += yaw_offset;

	const float forward_sensitivity = 0.01f;
	velocity += get_front() * forward_offset * forward_sensitivity;
}

void Player::add_gravity(glm::vec3 gravity) {
	velocity += gravity;
	last_gravity = gravity;
}

void Player::set_position(glm::vec3 position) {
	this->position = position;
}

glm::vec3 Player::get_position() {
	static float last_time = -1;
	float time = get_time();
	if (last_time < 0)
		last_time = time;
	float delta_time = time - last_time;
	last_time = time;

	const float multiplier = 512.0f;
	position += velocity * multiplier * delta_time;
	return position;
}

glm::vec3 Player::get_velocity() {
	return velocity;
}

void Player::get_camera_vecs(glm::vec3 *front, glm::vec3 *right, glm::vec3 *up) {
	*front = get_front();

	const glm::vec3 WORLDUP(0.0f, 1.0f, 0.0f);
	*right = glm::normalize(glm::cross(*front, WORLDUP));
	if (cos(pitch) < 0)
		*right *= (-1);

	*up = glm::normalize(glm::cross(*right, *front));
}

glm::vec3 Player::get_front() {
	glm::vec3 front;
	front.x = cos(yaw) * cos(pitch);
	front.y = sin(pitch);
	front.z = sin(yaw) * cos(pitch);
	front = glm::normalize(front);

	return front;
}

void Player::draw_lines(glm::mat4 projection, glm::mat4 view, glm::vec3 planet_pos) {
	const float multiplier = 5.0f;
	glm::vec3 front_pos = position + get_front();
	glm::vec3 velocity_pos = front_pos + velocity * multiplier;

	draw_line(projection, view, front_pos, velocity_pos, glm::vec4(1, 1, 0, 1)); //yellow
	draw_line(projection, view, velocity_pos, planet_pos, glm::vec4(1, 1, 1, 1)); //white
}
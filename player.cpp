#include "player.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Player::Player(glm::vec3 position, float pitch, float yaw) : position(position), pitch(pitch), yaw(yaw) {
	velocity = glm::vec3(0.0f);
}

void Player::process_input(float forward_offset, float pitch_offset, float yaw_offset) {
	pitch += pitch_offset;
	yaw += yaw_offset;

	const float forward_sensitivity = 0.01f;
	velocity += get_front() * forward_offset * forward_sensitivity;
	position += velocity;
}

void Player::set_position(glm::vec3 position) {
	this->position = position;
}

glm::vec3 Player::get_position() {
	return position;
}

void Player::get_camera_vecs(glm::vec3 *front, glm::vec3 *right, glm::vec3 *up) {
	*front = get_front();

	const glm::vec3 WORLDUP(0.0f, 1.0f, 0.0f);
	*right = glm::normalize(glm::cross(*front, WORLDUP));

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
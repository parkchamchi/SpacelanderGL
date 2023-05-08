#include "planet.hpp"

Planet::Planet(Drawable *drawable, Shader *shader) : drawable(drawable), shader(shader) {

}

Planet::~Planet() {

}

void Planet::draw(glm::mat4 projection, glm::mat4 view) {
	shader->use();

	shader->setMat4("projection", projection);
	shader->setMat4("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	shader->setMat4("model", model);

	drawable->draw(shader);
}
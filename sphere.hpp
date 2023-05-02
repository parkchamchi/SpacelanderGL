#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>

class GameObj {
public:
	//virtual void draw();
};

class Sphere : public GameObj {
public:
	Sphere(glm::vec3 loc);
	~Sphere();
	
	void draw(glm::mat4 proj, glm::mat4 view);

private:
	unsigned int VBO, VAO;
	Shader* shader;

	glm::vec3 location;
};

class Star : public Sphere {

};

class Planet : public Sphere {

};
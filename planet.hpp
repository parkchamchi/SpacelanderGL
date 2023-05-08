#include <glm/glm.hpp>
#include <learnopengl/shader_m.h>

class Drawable {
public:	
	virtual void draw(Shader *shader) = 0;
};

class Planet {
public:
	Planet(Drawable *drawable, Shader *shader);
	~Planet();

	void draw(glm::mat4 projection, glm::mat4 view);

private:
	Drawable *drawable;
	Shader *shader;
};
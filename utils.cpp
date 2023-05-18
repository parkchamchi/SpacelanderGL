#include "utils.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <learnopengl/shader_m.h>

#include <cmath>
#include <iostream>
#include <vector>
#include <tuple>

using namespace std;

class Textures {
public:
	Textures();
	~Textures();
	
	void add_texture(unsigned int texture, GLenum target);
	void use();

private:
	vector<tuple<unsigned int, GLenum>> *textures;
};

class Shape {
public:
	Shape(int vlen, GLenum mode);
	~Shape();

	virtual void draw(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float scale, glm::vec4 color);

protected:
	int get_vlen(); //# of vertices
	float *get_vertices(); //float[vlen * 3]
	void apply_vertices(); //Should be called after changing `vertices`

	void set_shader(Shader *shader); //Override the default shader
	//Shader *get_shader();
	void set_textures(Textures *textures);

private:
	GLenum mode;
	int vlen;

	float *vertices;
	unsigned int VAO, VBO;
	Shader *shader;

	Textures *textures;
};

class Circle : public Shape {
public:
	Circle();

	void draw(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius, glm::vec4 color);
};

class Line : public Shape {
public:
	Line();

	void draw(glm::mat4 projection, glm::mat4 view, glm::vec3 p, glm::vec3 q, glm::vec4 color);
};

class Cube : public Shape {
public:
	Cube();
};

class Cubemap : public Cube {
public:
	Cubemap(unsigned int texture);
	~Cubemap();

	void draw(glm::mat4 projection, glm::mat4 view);

	unsigned int get_texture() const;
	bool operator==(const Cubemap &o) const;

private:
	unsigned int texture; //for operator==
	Textures *textures;
};

static Circle *circle = nullptr;
static Line *line = nullptr;
static Cube *cube = nullptr;
static Cubemap *cubemap = nullptr;

void draw_circle(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius, glm::vec4 color) {
	if (circle == nullptr)
		circle = new Circle();

	circle->draw(projection, view, location, radius, color);
}

void draw_line(glm::mat4 projection, glm::mat4 view, glm::vec3 p, glm::vec3 q, glm::vec4 color) {
	if (line == nullptr)
		line = new Line();

	line->draw(projection, view, p, q, color);
}

void draw_cube(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float size, glm::vec4 color) {
	if (cube == nullptr)
		cube = new Cube();

	cube->draw(projection, view, location, size, color);
}

void draw_cubemap(glm::mat4 projection, glm::mat4 view, unsigned int cubemap_texture) {
	if (cubemap != nullptr && cubemap->get_texture() != cubemap_texture)
		cubemap = nullptr;
	if (cubemap == nullptr)
		cubemap = new Cubemap(cubemap_texture);

	cubemap->draw(projection, view);
}

void utils_cleanup() {
	if (circle != nullptr)
		delete circle;
	if (line != nullptr)
		delete line;
	if (cube != nullptr)
		delete cube;
	if (cubemap != nullptr)
		delete cubemap;
}

Textures::Textures() {
	textures = new vector<tuple<unsigned int, GLenum>>();
}

void Textures::add_texture(unsigned int texture, GLenum target) {
	textures->push_back(make_tuple(texture, target));
}

void Textures::use() {
	int i = 0;
	for (auto tex_and_mode : *textures) {
		glActiveTexture(GL_TEXTURE0 + (i++));
		glBindTexture(get<1>(tex_and_mode), get<0>(tex_and_mode));
	}
}

Textures::~Textures() {
	delete textures;
}

Shape::Shape(int vlen, GLenum mode) : vlen(vlen), mode(mode) {
	//glBindVertexArray(0); //Unbind
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Set VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, 3 * vlen * sizeof (float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), (void *) 0);
	glEnableVertexAttribArray(0);

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); //Unbind

	//Set shader
	shader = new Shader("shaders/monocolor.vs", "shaders/monocolor.fs");

	//Set vertices
	vertices = new float[vlen*3];

	textures = nullptr;
}

int Shape::get_vlen() {
	return vlen;
}

float *Shape::get_vertices() {
	return vertices;
}

void Shape::apply_vertices() {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vlen * 3 * sizeof(float), vertices, GL_STATIC_DRAW);
}

void Shape::set_shader(Shader *shader) {
	this->shader = shader;
}

/*
Shader *Shape::get_shader() {
	return shader;
}
*/

void Shape::set_textures(Textures *textures) {
	this->textures = textures;
}

void Shape::draw(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float scale, glm::vec4 color) {
	shader->use();
	shader->setVec4("aColor", color);

	shader->setMat4("projection", projection);
	shader->setMat4("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, location);
	model = glm::scale(model, glm::vec3(scale));
	shader->setMat4("model", model);

	glBindVertexArray(VAO);
	if (textures != nullptr) {
		textures->use(); 
	}
	glDrawArrays(mode, 0, vlen);	
}

Shape::~Shape() {
	delete shader;
	delete[] vertices;
	glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

Circle::Circle() : Shape(360, GL_LINE_LOOP) {
	int vlen = get_vlen();

	//Set `vertices`
	float *vertices = get_vertices();
	for (int i = 0; i < vlen; i++) {
		float angle = ((float) i / vlen) * 2*PI;
		vertices[i*3 + 0] = cos(angle);
		vertices[i*3 + 1] = 0.0f;
		vertices[i*3 + 2] = sin(angle);
	}
	apply_vertices();
}

void Circle::draw(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius, glm::vec4 color) {
	Shape::draw(projection, view, location, radius, color);
}

Line::Line() : Shape(2, GL_LINES) {
	int vlen = get_vlen();
	float *vertices = get_vertices();
	for (int i = 0; i < vlen*3; i++)
		vertices[i] = 0;
	apply_vertices();
}

void Line::draw(glm::mat4 projection, glm::mat4 view, glm::vec3 p, glm::vec3 q, glm::vec4 color) {
	float *vertices = get_vertices();
	vertices[0] = p.x;
	vertices[1] = p.y;
	vertices[2] = p.z;
	vertices[3] = q.x;
	vertices[4] = q.y;
	vertices[5] = q.z;
	apply_vertices();

	Shape::draw(projection, view, glm::vec3(0), 1, color);
}

Cube::Cube() : Shape(36, GL_TRIANGLES) {
	int vlen = get_vlen();
	float *vertices = get_vertices();
	
	float new_vertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
	memcpy(vertices, new_vertices, sizeof (new_vertices));
	apply_vertices();
}

Cubemap::Cubemap(unsigned int texture) : Cube() {
	Shader *shader = new Shader("shaders/cubemap.vs", "shaders/cubemap.fs");
	shader->use();
	shader->setInt("skybox", 0);
	set_shader(shader);
	
	textures = new Textures();
	textures->add_texture(texture, GL_TEXTURE_CUBE_MAP);
	set_textures(textures);
	this->texture = texture;
}

void Cubemap::draw(glm::mat4 projection, glm::mat4 view) {
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	Cube::draw(projection, glm::mat3(view), glm::vec3(10, 10, 10), 1, glm::vec4(1)); //all vars except proj and view are dummy vars + remove trans.
	glDepthFunc(GL_LESS); // set depth function back to default
}

bool Cubemap::operator==(const Cubemap &o) const {
	return this->get_texture() == o.get_texture();
}

unsigned int Cubemap::get_texture() const {
	return texture;
}

Cubemap::~Cubemap() {
	delete textures;
}
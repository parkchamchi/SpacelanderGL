#ifndef UTILS_HPP
#define UTILS_HPP

#include <glm/glm.hpp>

const float PI = 3.1415926f;

float get_time();

void draw_circle(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float radius, glm::vec4 color=glm::vec4(0, 1, 0, 1));
void draw_line(glm::mat4 projection, glm::mat4 view, glm::vec3 p, glm::vec3 q, glm::vec4 color=glm::vec4(0, 1, 0, 1));
void draw_cube(glm::mat4 projection, glm::mat4 view, glm::vec3 location, float size=1.0f, glm::vec4 color=glm::vec4(0, 1, 0, 1));
void draw_cubemap(glm::mat4 projection, glm::mat4 view, unsigned int cubemap_texture);
void utils_cleanup();

#endif
#ifndef UTILS_HPP
#define UTILS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

using namespace std;

string vec3_tostring(glm::vec3 vec) {
	return "vec3<" + to_string(vec.x) + ", " + to_string(vec.y) + ", " + to_string(vec.z) + ">";
}

#endif
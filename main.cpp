#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "planet.hpp"
#include "player.hpp"
#include "utils.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow *window);
//unsigned int loadTexture(const char *path);
unsigned int load_cubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0, 0, 5), glm::vec3(0, 1, 0), 0, 0);
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//bool firstMouse = true;

Player player(glm::vec3(0.0f), 0.0f, 90.0f);

// timing
float delta_time = 0.0f;
float last_frame = 0.0f;

bool landed = false;

//To bypass the stbi error (Should be included in only one translation unit)
class DrawableModel : public Drawable {
public:
	DrawableModel(Model *model) : model(model) {}
	void draw(Shader *shader) override {model->Draw(*shader);}
private:
	Model *model;
};

int main() {
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SpacelanderGL", NULL, NULL);
	if (window == NULL)	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	//For some reason turning on flips the cubemaps' textures. I assume this also flips the planet's texture...
	//To bypass this, just set this on `load_cubemap()` function.
    //stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	Shader planet_shader("shaders/planet.vs", "shaders/planet.fs");
	Model planet_model("resources/mars/mars.obj");
	DrawableModel planet_drawable_model(&planet_model);

	Planet planet(
		&planet_drawable_model, &planet_shader,
		5.0f, //radius
		0.01f, //orbit_freq
		100.0f, //orbit_radius
		0.05f, //rot_freq
		glm::vec3(0.0f, 1.0f, 0.0f), //rot_axis
		
		[](Shader *shader, Planet *planet) {
			shader->setInt("material.diffuse", 0);
			shader->setInt("material.specular", 0);
			shader->setFloat("material.shininess", 1.0f);

			shader->setVec3("viewPos", camera.Position);

			shader->setVec3("dirLight.direction", planet->get_position());
			shader->setVec3("dirLight.ambient", glm::vec3(0.2f));
			shader->setVec3("dirLight.diffuse", glm::vec3(1.0f));
			shader->setVec3("dirLight.specular", glm::vec3(0.1f));

			shader->setVec3("spotLight.position", camera.Position);
			shader->setVec3("spotLight.direction", camera.Front);
			shader->setVec3("spotLight.ambient", glm::vec3(0.0f));
			shader->setVec3("spotLight.diffuse", glm::vec3(0.05f));
			shader->setVec3("spotLight.specular", glm::vec3(0.0f));
			shader->setFloat("spotLight.constant", 1.0f);
			shader->setFloat("spotLight.linear", 0.045f);
			shader->setFloat("spotLight.quadratic", 0.0075f);
			shader->setFloat("spotLight.cutOff", glm::cos(glm::radians(15.0f)));
			shader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
		}
	);

	//Init. camera
	planet.update();
	glm::vec3 initial_position = planet.get_position() + glm::vec3(0.0f, 1.0f, -2 * planet.get_radius());
	camera.Position = initial_position;
	player.set_position(initial_position + glm::vec3(0, 4, 0));
	player.get_camera_vecs(&camera.Front, &camera.Right, &camera.Up);

	//Set cubemap texture
	vector<std::string> faces {
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg"
    };
    unsigned int cubemap_texture = load_cubemap(faces);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		// --------------------
		float current_frame = static_cast<float>(glfwGetTime());
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		// input
		// -----
		process_input(window);

		// render
		// ------

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 gravity = planet.get_gravity(player.get_position(), delta_time);
		player.add_gravity(gravity);
		camera.Position = player.get_position();
		player.get_camera_vecs(&camera.Front, &camera.Right, &camera.Up);

		const float NEAR = 0.05f;
		const float FAR = 512.0f;
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / SCR_HEIGHT, NEAR, FAR);
		glm::mat4 view = camera.GetViewMatrix();	

		planet.update();
		planet.draw(projection, view);
		player.draw_lines(projection, view, planet.get_position());

		draw_cubemap(projection, view, cubemap_texture);

		//Calculate the distance
		if (!landed) {
			glm::vec3 player_to_planet = planet.get_position() - player.get_position();
			glm::vec3 land_velocity = player.get_velocity();

			float dist = glm::length(player_to_planet) - planet.get_radius();
			float speed = glm::length(land_velocity);

			//Angle between the player's velocity and the direction beween the player and the planet. Degrees.
			float angle = glm::degrees(glm::acos(
				glm::dot(glm::normalize(land_velocity), glm::normalize(player_to_planet))));

			//Print
			if (dist > NEAR*2)
				//std::cout << '\r' << "Distance: " << dist << ", Speed: " << speed << ", Angle: " << angle;
				printf("\rDistance: %.2f, Speed: %.4f, Angle: %.2f", dist, speed, angle);
			else {
				landed = true; //End

				/*
				Score using the values on the land
				Speed: `100` on 0.01, `0` on 0.1
				Angle: `100` on 0, `0` on 360.
				*/
				float speed_score = -1000 * speed + 110;
				if (speed_score > 100) speed_score = 100;
				if (speed_score < 0) speed_score = 0;

				float angle_score = (-5.0f/18) * angle + 100;

				putchar('\n');
				printf("Speed score: %.2f\n", speed_score);
				printf("Angle score: %.2f\n", angle_score);

				const float speed_score_weight = 3;
				const float angle_score_weight = 1;
				float total_score = (speed_score * speed_score_weight + angle_score * angle_score_weight) 
					/ (speed_score_weight + angle_score_weight);
				printf("Total score (weights: (%.1f, %.1f)): %.2f\n", speed_score_weight, angle_score_weight, total_score); 
			}
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------

	glfwTerminate();
	utils_cleanup();
	std::cout << "\nExiting." << std::endl;
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void process_input(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float forward_offset = 0.0f, pitch_offset = 0.0f, yaw_offset = 0.0f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		pitch_offset += delta_time;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		pitch_offset -= delta_time;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		yaw_offset -= delta_time;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		yaw_offset += delta_time;
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
		forward_offset -= delta_time;
	if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
		forward_offset += delta_time;

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		landed = false; //Reset

	player.process_input(forward_offset, pitch_offset, yaw_offset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
/*void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	//camera.ProcessMouseMovement(xoffset, yoffset);
}
*/

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
/*
unsigned int loadTexture(char const * path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
*/

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int load_cubemap(vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
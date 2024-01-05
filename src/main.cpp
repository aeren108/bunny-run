#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "global.h"
#include "model.h"
#include "bunny.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

int gWidth, gHeight;

glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::vec3 eyePos(0, 0, 0);

Model ground;
Bunny* bunny;


void init() {
    glEnable(GL_DEPTH_TEST);
	
	bunny = new Bunny();

	ground = Model("assets/quad.obj", "shaders/frag.glsl", "shaders/vert.glsl");
	ground.pos = glm::vec3(0.f, -1.f, -50.f);
	ground.scale = glm::vec3(2.f, 10.f, 50.f);
	ground.rotaxis = glm::vec3(1.f, 0.f, 0.f);
	ground.rotangle = - M_PI / 2;
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

void reshape(GLFWwindow* window, int w, int h) {
	w = w < 1 ? 1 : w;
	h = h < 1 ? 1 : h;

	gWidth = w;
	gHeight = h;

	glViewport(0, 0, w, h);

	// Use perspective projection
	float fovyRad = (float)(90.0 / 180.0) * M_PI;
	projectionMatrix = glm::perspective(fovyRad, w / (float) h, 1.0f, 100.0f);

	// Assume default camera position and orientation (camera is at
	// (0, 0, 0) with looking at -z direction and its up vector pointing
	// at +y direction)
	// 
	//viewingMatrix = glm::mat4(1);
	viewingMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
}

void checkCollisions() {
	// TODO check collisions between bunny and checkpoints
}

void update() {
	bunny->update();
}

void render() {
    glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    /*glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.f, -3.f));
	glm::mat4 matS = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5));
	glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), (-180. / 180.) * M_PI, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 matRz = glm::rotate(glm::mat4(1.0), angleRad, glm::vec3(0.0, 0.0, 1.0));
	modelingMatrix = matT * matRz * matR;*/

	ground.render();
    bunny->render();
}

void gameLoop(GLFWwindow* window) {
    while (!glfwWindowShouldClose(window)) {
        update();
		render();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

int main(int argc, char** argv) {
	GLFWwindow* window;
	if (!glfwInit()) {
		exit(-1);
	}

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	int width = 1000, height = 800;
	window = glfwCreateWindow(width, height, "Simple Example", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	char rendererInfo[512] = { 0 };
	strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER)); // Use strcpy_s on Windows, strcpy on Linux
	strcat(rendererInfo, " - "); // Use strcpy_s on Windows, strcpy on Linux
	strcat(rendererInfo, (const char*)glGetString(GL_VERSION)); // Use strcpy_s on Windows, strcpy on Linux
	glfwSetWindowTitle(window, rendererInfo);

	init();

	glfwSetKeyCallback(window, keyboard);
	glfwSetWindowSizeCallback(window, reshape);

	reshape(window, width, height); // need to call this once ourselves
	gameLoop(window); // this does not return unless the window is closed

	glfwDestroyWindow(window);
	glfwTerminate();

	delete bunny;

	return 0;
}


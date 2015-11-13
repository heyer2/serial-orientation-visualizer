#include <stdio.h>
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include "headers/interface.h"

#define WINDOW_NAME "Orientation Visualizer"
#define WINDOW_DEF_WIDTH  500
#define WINDOW_DEF_HEIGHT 500

#define ERR_GLFWCALL "Error: GLFW callback error, see below:\n"

void callbackError(int error, const char* description)
{
    fputs(ERR_GLFWCALL, stderr);
    fputs(description, stderr);
}

void callbackClose(GLFWwindow * window)
{	
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void callbackKey(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void callbackFramebufferSize(GLFWwindow * window, int width, int height) {
	int minSize = width >= height ? height : width;
	glViewport((width - minSize) / 2, (height - minSize) / 2, minSize, minSize);
}

void callbackWindowSize(GLFWwindow * window, int width, int height)
{	
	int minSize = width >= height ? height : width;
	glfwSetWindowSize(window, minSize, minSize);
}

void setCallbacks(GLFWwindow * window)
{
	glfwSetErrorCallback(callbackError);
	glfwSetWindowCloseCallback(window, callbackClose); 
	glfwSetKeyCallback(window, callbackKey);
	glfwSetFramebufferSizeCallback(window, callbackFramebufferSize);
	glfwSetWindowSizeCallback(window, callbackWindowSize);
}

GLFWwindow* interfaceInitWindow(void)
{
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, 100);
	GLFWwindow* window = glfwCreateWindow(WINDOW_DEF_WIDTH, WINDOW_DEF_HEIGHT, WINDOW_NAME, NULL, NULL);
	setCallbacks(window);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	glViewport(0, 0, bufferWidth,bufferHeight);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	return window;
}

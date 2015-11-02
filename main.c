#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "glew/glew.h"
#include "glfw/glfw3.h"
#include "headers/serial.h"
#include "headers/graphics.h"
#include "headers/matrix4.h"

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

#define PROXIMITY_STEPSIZE 0.5;
#define HEADING_STEPSIZE (3.14159265359/16);
#define SCALING_STEPSIZE 0.2;

	
double cubeProximity = -2;
double cubeHeading = 0;
double cubeScaleX = 1;
double cubeScaleY = 1;
double cubeScaleZ = 1;

void SIGINT_handler(int sig)
{
	signal(sig, SIG_IGN);
	glfwTerminate();
	serialEnd();
	printf("\nVisualization aborted.\n");
	exit(0);
}

void error_callback(int error, const char* description)
{
    fputs("GLFW related error.\n", stderr);
}

void close_callback(GLFWwindow* window)
{	
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
		
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		cubeProximity -= PROXIMITY_STEPSIZE;
	
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		cubeProximity += PROXIMITY_STEPSIZE;
		
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		cubeHeading -= HEADING_STEPSIZE;
	
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		cubeHeading += HEADING_STEPSIZE;
		
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
		cubeScaleX += SCALING_STEPSIZE;
	
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
		cubeScaleX -= SCALING_STEPSIZE;
	
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
		cubeScaleY += SCALING_STEPSIZE;
	
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
		cubeScaleY -= SCALING_STEPSIZE;
	
	if (key == GLFW_KEY_U && action == GLFW_PRESS)
		cubeScaleZ += SCALING_STEPSIZE;
	
	if (key == GLFW_KEY_J && action == GLFW_PRESS)
		cubeScaleZ -= SCALING_STEPSIZE;
}

// This is redundant, but nice to have
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	int minSize = width >= height ? height : width;
	glViewport((width-minSize)/2, (height-minSize)/2, minSize, minSize);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	
	int minSize = width >= height ? height : width;
	glfwSetWindowSize(window, minSize, minSize);
}

void setCallbacks(GLFWwindow* window) {
	glfwSetErrorCallback(error_callback);
	glfwSetWindowCloseCallback(window, close_callback); 
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
}

GLFWwindow* initWindow(void) {
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, 200); // SHOULD REMOVE?
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Orientation Visualizer", NULL, NULL);
	setCallbacks(window);
 	
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1); //DANGER?
	glViewport(0, 0, bufferWidth,bufferHeight);
	glewExperimental = GL_TRUE;
	glewInit();	
	
	graphicsInit();
	
	return window;
}

int main(int argc, char** argv)
{	

	signal(SIGINT, SIGINT_handler);
	struct serialPort serialPort1;
	
	if (argc == 4) {
		serialStartArgs(&serialPort1, argv + 1);
	}
	else if (argc == 1) {
		serialStartManual(&serialPort1);
	}
	else {
		fputs("Error: Incorrect number of arguments.\n", stderr);
		exit(EXIT_FAILURE);
	}
	
    if (!glfwInit())
        exit(EXIT_FAILURE);
	GLFWwindow* window = initWindow();
	
	struct mat4 serialMatrix;
	struct mat4 guiMatrix;
	struct mat4 modelMatrix;
	struct mat4 scalingMatrix;
	
	mat4Perspective(&graphicsProjectionMatrix, 1, 5, 1, 1);
	
	mat4Init(&serialMatrix);
	mat4Init(&guiMatrix);
	mat4Init(&modelMatrix);
	mat4Init(&scalingMatrix);
	
	mat4SetScaling(&scalingMatrix, cubeScaleX, cubeScaleY, cubeScaleZ);
	
	printf("Read: %i, write: %i, line: %i\n", serialPort1.readIdx, serialPort1.writeIdx, serialPort1.lineIdx);

	double packetTime = glfwGetTime();
	double frameTime = 	glfwGetTime();
	double avgPacketTime = 10000;;
	int updated;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (updated) {
			printf("Frequency: %f\n", 1 / avgPacketTime);
			avgPacketTime = avgPacketTime * 0.95 + (glfwGetTime() - packetTime) * 0.05;
			packetTime = glfwGetTime();
		}

		updated = serialUpdateBuffer(&serialPort1);
		serialUpdataOrientation(&serialPort1, &serialMatrix);
		mat4SetRotFromHPR(&guiMatrix, cubeHeading, 0, 0);
		mat4SetTranslation(&guiMatrix, cubeProximity, 0, 0);
		
		mat4Mult(&serialMatrix, &scalingMatrix, &modelMatrix);
		mat4Mult(&guiMatrix, &modelMatrix, &modelMatrix);
		
		if (glfwGetTime() - frameTime > 1.0f / 60) {
			graphicsDrawCube(&modelMatrix);		
			glfwSwapBuffers(window);
			frameTime = glfwGetTime();
		}
	}
	
	printf("Visualization finished.\n");
	glfwTerminate();
	serialClose(&serialPort1);
	serialEnd();
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
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

// Simple matrix stuff


void error_callback(int error, const char* description)
{
    fputs("GLFW related error!", stderr);
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


int main()
{	
	//printf("%i",comEnumerate());
	struct serialPort serialPort1;
	serialStart(&serialPort1);
	
    if (!glfwInit())
        exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Orientation Visualizer", NULL, NULL);
	setCallbacks(window);
 	
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(window);
	glViewport(0, 0, bufferWidth,bufferHeight);
	
	glewExperimental = GL_TRUE;
	glewInit();	
	graphicsInit();
	
	struct mat4 serialMatrix;
	struct mat4 guiMatrix;
	struct mat4 modelMatrix;
	struct mat4 scalingMatrix;
	
	mat4Perspective(&graphicsProjectionMatrix, 1, 5, 1, 1);
	//mat4Disp(&graphicsProjectionMatrix, "projection matrix init");
	//printf("%i\n", comEnumerate());
	
	
	mat4Init(&serialMatrix);
	mat4Init(&guiMatrix);
	mat4Init(&modelMatrix);
	mat4Init(&scalingMatrix);
	
	mat4SetScaling(&scalingMatrix, cubeScaleX, cubeScaleY, cubeScaleZ);
	
	printf("Read: %i, write: %i, line: %i\n", serialPort1.readIdx, serialPort1.writeIdx, serialPort1.lineIdx);
	//mat4Disp(&modelMatrix, "model matrix");
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		//printf("%i\n", serialUpdateBuffer(&serialPort1));
		serialUpdateBuffer(&serialPort1);
		//printf("%i\n", test);
		
		printf("Read idx: %i Write idx: %i \n", serialPort1.readIdx, serialPort1.writeIdx);
		if (serialUpdataOrientation(&serialPort1, &serialMatrix))
			printf("Received data.\n");
		mat4SetRotFromHPR(&guiMatrix, cubeHeading, 0, 0);
		mat4SetTranslation(&guiMatrix, cubeProximity, 0, 0);
		
		mat4Mult(&serialMatrix, &scalingMatrix, &modelMatrix);
		mat4Mult(&guiMatrix, &modelMatrix, &modelMatrix);
		
			
		graphicsDrawCube(&modelMatrix);		
		glfwSwapBuffers(window);
	}
	printf("Visualization finished.\n");
	glfwTerminate();
	serialClose(&serialPort1);
	serialEnd();
	return 0;
}

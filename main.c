#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include "headers/serial.h"
#include "headers/graphics.h"
#include "headers/matrix4.h"
#include "headers/interface.h"

#define ERR_GLFW "Error: GLFW related error.\n"
#define INFO_END "\nVisualization over.\n"


#define UPDATESPEED_PACKAGE 0.999
#define UPDATESPEED_FRAME 0.99

#define FRAMERATE_TARGET 60.0

#define TIMEOUT_PACKAGE 4

static void progExit(void)
{
	graphicsExit();
	glfwTerminate();
	serialEnd();
	fputs(INFO_END, stdout);
	exit(0);
}

static void signalIntHandler(int sig)
{
	signal(sig, SIG_IGN);
	progExit();
}

static double timeSince(double time)
{
	return glfwGetTime() - time;
}

int main(int argc, char** argv)
{	
	// Initialization
   if (!glfwInit()) {
    	fputs(ERR_GLFW, stdout);
        exit(EXIT_FAILURE);
    }
	signal(SIGINT, signalIntHandler);
	GLFWwindow * window = interfaceInitWindow();
	graphicsInit();
	struct serialPort serialPort1;
	serialInitiate(&serialPort1, argc - 1, argv + 1);
    struct graphicsConfig graCon;
    graphicsConfigSetDefault(&graCon);
	double timerPackage = glfwGetTime();
	double timerFrame   = glfwGetTime();
	double intervalPackage  = 0;
	double intervalFrame    = 0;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		int countPackages = serialUpdate(&serialPort1);

		if (countPackages) {
			double intervalDiff = timeSince(timerPackage) - intervalPackage;
			intervalPackage += UPDATESPEED_PACKAGE * intervalDiff / countPackages;
			timerPackage = glfwGetTime();
		}

		if (timeSince(timerPackage) > TIMEOUT_PACKAGE) {
			intervalPackage += timeSince(timerPackage);
			timerPackage = glfwGetTime();
		}

		printf("Frame: %f Framterval: %f package %f", 1 / intervalFrame, intervalFrame, 1 / intervalPackage);
		printf("packages yo %i\n", countPackages);


		if (timeSince(timerFrame) > 1.0 / FRAMERATE_TARGET) {
			double intervalDiff = timeSince(timerFrame) - intervalFrame;
			intervalFrame += UPDATESPEED_FRAME * intervalDiff;
			timerFrame = glfwGetTime();
			graCon.matSerial = serialPort1.matOri;
			graphicsDrawCube(&graCon);		
			glfwSwapBuffers(window);
			serialPort1.flagNewOrientation = 0;
		}

	}
	
	progExit();
	return 0; // Unneccesary
}

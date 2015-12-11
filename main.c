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

#define UPDATESPEED_FRAME 0.99
#define FRAMERATE_TARGET 60.0

#define SERIAL_POOL_INTERVAL (1.0/10000.0)
#define PACKAGE_SPEED_DISP_INTERVAL 1.0


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
	double timerPool    = glfwGetTime();
	double freqPackage = 0;
	double intervalFrame = 0;
	int totalPackages = 0;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (timeSince(timerPool) > SERIAL_POOL_INTERVAL) {
			timerPool = glfwGetTime();
			totalPackages += serialUpdate(&serialPort1);
		}

		if (timeSince(timerPackage) > PACKAGE_SPEED_DISP_INTERVAL) {
			freqPackage = totalPackages / PACKAGE_SPEED_DISP_INTERVAL;
			timerPackage = glfwGetTime();
			totalPackages = 0;
		}

		if (timeSince(timerFrame) > 1.0 / FRAMERATE_TARGET) {
			double intervalDiff = timeSince(timerFrame) - intervalFrame;
			intervalFrame += UPDATESPEED_FRAME * intervalDiff;
			timerFrame = glfwGetTime();
			graCon.matSerial = serialPort1.matOri;
			graphicsDrawCube(&graCon);		
			glfwSwapBuffers(window);
			serialPort1.flagNewOrientation = 0;
			interfaceUpdateTitle(window, 1.0 / intervalFrame, freqPackage, mat4Det(&serialPort1.matOri));
		}
	}
	
	progExit();
	return 0; // Redundant
}

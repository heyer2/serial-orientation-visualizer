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
	double timerPool    = glfwGetTime();
	double freqPackage = 0;
	double intervalFrame = 0;
	int totalPackages = 0;

	

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();


		if (timeSince(timerPool) > 1.0/10000.0) {
			int countPackages = serialUpdate(&serialPort1);
			timerPool = glfwGetTime();
			totalPackages += countPackages;
		}

		/*s
		static double timerDeterminant = glfwGetTime();
		static int bad = 0;
		if (timeSince(timerDeterminant) > 60) {
			timerDeterminant = glfwGetTime();
			printf("Bad: %i\n", bad / 60.0f);
			bad = 0;
		}
		*/

		if (timeSince(timerPackage) > 2) {
			freqPackage = totalPackages / 2;
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
			printf("Frame: %f package: %f\n\r" , 1 / intervalFrame, freqPackage);
		}
	}
	
	progExit();
	return 0; // Redundant
}

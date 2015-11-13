#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <glfw/glfw3.h>
#include "matrix4.h"

struct graphicsConfig {
	struct mat4 matSerial;
	double scaleX;
	double scaleY;
	double scaleZ;
	double camDist;
	double projNear;
	double projFar;
	double projWidth;
	double projHeight;
};

void graphicsConfigSetDefault(struct graphicsConfig * graCon);
void graphicsDrawCube(struct graphicsConfig * graCon);
void graphicsInit(void);
void graphicsExit(void);

#endif /* GRAPHICS_H */

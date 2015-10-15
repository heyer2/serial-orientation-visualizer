#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "matrix4.h"

struct mat4 graphicsProjectionMatrix;
void graphicsDrawCube(struct mat4* modelMatrix);
void graphicsInit(void);
void graphicsExit(void);

#endif /* GRAPHICS_H */
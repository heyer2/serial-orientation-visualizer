#ifndef INTERFACE_H
#define INTERFACE_H

#include <glfw/glfw3.h>

GLFWwindow* interfaceInitWindow(void);
void interfaceUpdateTitle(GLFWwindow * window, double frame, double package, double det);

#endif /* INTERFACE_H */

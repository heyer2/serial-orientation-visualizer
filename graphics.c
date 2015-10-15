#include <stdlib.h>
#include <stdio.h>
#include "glew/glew.h"
#include "headers/matrix4.h"
#include "headers/graphics.h"

// Draw settings - remembers necessary stuff for drawing

GLuint cubeShader;
GLuint cubeVAO;
GLuint cubeVBO;
GLuint cubeEBO;
GLuint cubeModelUni;
GLuint cubeProjUni;

// Shaders
const GLchar* const vertexShaderSource =
	"#version 330 core                                                    \n"
	"                                                                     \n"
	"layout (location = 0) in vec3 vertexPos;                             \n"
	"layout (location = 1) in vec3 vertexColor;                           \n"
	"                                                                     \n"
	"out vec3 fragColor;												  \n"
	"																      \n"
	"uniform mat4 modelMatrix;                                            \n"
	"uniform mat4 projMatrix;                                             \n"
	"                                                                     \n"
	"void main()                                                          \n"
	"{                                                                    \n"
			"	vec4 tmpPos = modelMatrix * vec4(vertexPos, 1.0f);        \n"
			"	gl_Position = projMatrix * tmpPos.yzxw;     	          \n"
	"	fragColor= vertexColor;     	                                  \n"
	"}                                                                    \n"; 

const GLchar* const fragmentShaderSource =
	"#version 330 core                                                    \n"
	"                                                                     \n"
	"in  vec3 fragColor;                                                  \n"
	"												                      \n"
	"out vec4 color;                                                      \n"
	"                                                                     \n"
	"void main()                                                          \n"
	"{                                                               	  \n"
	"	color = vec4(fragColor, 1.0f);                                    \n"
	"}                                                               	  \n";
	
// 3D model
	
const GLfloat cubeVertices[] = { // x y z, r g b
    -0.4f, -0.2f, -0.2f,  0.0f,  0.6f,  0.0f, // 0 Near-Bottom-Left
    -0.4f,  0.2f, -0.2f,  0.0f,  0.6f,  0.0f, // 1 Near-Bottom-Right
    -0.4f, -0.2f,  0.2f,  1.0f,  0.0f,  0.0f, // 2 Near-Top-Left
	-0.4f,  0.2f,  0.2f,  1.0f,  0.0f,  0.0f, // 3 Near-Top-Right
	 0.4f, -0.2f, -0.2f,  0.0f,  0.6f,  0.0f, // 4 Far-Bottom-Left
     0.4f,  0.2f, -0.2f,  0.0f,  0.6f,  0.0f, // 5 Far-Bottom-Right
     0.4f, -0.2f,  0.2f,  1.0f,  0.0f,  0.0f, // 6 Far-Top-Left
	 0.4f,  0.2f,  0.2f,  1.0f,  0.0f,  0.0f  // 7 Far-Top-Right
};

	
const GLuint cubeIndices[] = {
	0, 1, 4,
	1, 4, 5, // Bottom Surface
	2, 3, 6,
	3, 6, 7, // Top Surface
	0, 2, 4,
	2, 6, 4, // Left Surface
	1, 5, 3,
	3, 7, 5, // Right Surface
	0, 1, 2, 
    1, 2, 3, // Near Surface
	4, 5, 6, 
    5, 6, 7, // Far Surface
};  

// Initiation

	
GLuint initShaders(void)
{
	GLint success;
	GLchar infoLog[512];
	
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	
	if(!success){
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("Error in vertex shader compilation\n");
		printf("%s", infoLog);
		exit(EXIT_FAILURE);
	}
	
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		
	if(!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("Error in fragment shader compilation\n");
		printf("%s", infoLog);
		exit(EXIT_FAILURE);
	}
	
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	
	if(!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Error in shader linking\n");
		printf("%s", infoLog);
		exit(EXIT_FAILURE);
	}
	
	cubeModelUni = glGetUniformLocation(shaderProgram, "modelMatrix");
	cubeProjUni = glGetUniformLocation(shaderProgram, "projMatrix");
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);  
	return shaderProgram;
}


void initObjects(void)
{
	glGenVertexArrays(1, &cubeVAO);  
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);
	
	glBindVertexArray(cubeVAO);	
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO); 
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW); 
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void graphicsInit(void)
{	
	cubeShader = initShaders();
	initObjects();
	
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

}

void graphicsExit(void)
{
	glDeleteVertexArrays(1, &cubeEBO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
}

void fillGLfloatArrayFromMat4(struct mat4* inputMatrix, GLfloat outputArray[16])
{
	for (int i = 0; i < 16; i++)
		outputArray[i] = ((double*) inputMatrix)[i];
}

void graphicsDrawCube(struct mat4* modelMatrix)
{		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		GLfloat modelArray[16];
		GLfloat projArray[16];
		fillGLfloatArrayFromMat4(modelMatrix, modelArray);
		fillGLfloatArrayFromMat4(&graphicsProjectionMatrix, projArray);
		glUseProgram(cubeShader);
		glUniformMatrix4fv(cubeModelUni, 1, GL_TRUE, modelArray);
		glUniformMatrix4fv(cubeProjUni, 1, GL_TRUE, projArray);
		glBindVertexArray(cubeVAO);
		glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(*cubeIndices), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
		
}
//sizeof(cubeIndices) / sizeof(*cubeIndices)
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include "headers/matrix4.h"
#include "headers/graphics.h"

#define PROJ_DEF_NEAR  1
#define PROJ_DEF_FAR   5
#define PROJ_DEF_WIDTH 1
#define PROJ_DEF_HEIGHT 1

#define SCALE_DEF_X 2
#define SCALE_DEF_Y 1
#define SCALE_DEF_Z 0.25

#define CAMDIST_DEF (-2)


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
	
// Model
const GLfloat cubeVertices[] = { // x y z, r g b
    -0.2f, -0.2f,  0.2f,  0.0f,  0.0f,  0.5f, //  0 UpperSurf : Top-Near-Left
	-0.2f,  0.2f,  0.2f,  0.0f,  0.0f,  0.5f, //  1 UpperSurf : Top-Near-Right
	 0.2f, -0.2f,  0.2f,  0.0f,  0.0f,  0.5f, //  2 UpperSurf : Top-Far-Left
	 0.2f,  0.2f,  0.2f,  0.0f,  0.0f,  0.5f, //  3 UpperSurf : Top-Far-Right
    -0.2f, -0.2f, -0.2f,  0.0f,  0.0f,  0.5f, //  4 BotSurf   : Bottom-Near-Left
    -0.2f,  0.2f, -0.2f,  0.0f,  0.0f,  0.5f, //  5 BotSurf   : Bottom-Near-Right
	 0.2f, -0.2f, -0.2f,  0.0f,  0.0f,  0.5f, //  6 BotSurf   : Bottom-Far-Left
     0.2f,  0.2f, -0.2f,  0.0f,  0.0f,  0.5f, //  7 BotSurf   : Bottom-Far-Right
    -0.2f, -0.2f, -0.2f,  0.0f,  0.5f,  0.0f, //  8 LeftSurf  : Bottom-Near-Left
    -0.2f, -0.2f,  0.2f,  0.0f,  0.5f,  0.0f, //  9 LeftSurf  : Top-Near-Left
   	 0.2f, -0.2f, -0.2f,  0.0f,  0.5f,  0.0f, // 10 LeftSurf  : Bottom-Far-Left
   	 0.2f, -0.2f,  0.2f,  0.0f,  0.5f,  0.0f, // 11 LeftSurf  : Top-Far-Left
    -0.2f,  0.2f, -0.2f,  0.0f,  0.5f,  0.0f, // 12 RightSurf : Bottom-Near-Right
    -0.2f,  0.2f,  0.2f,  0.0f,  0.5f,  0.0f, // 13 RightSurf : Top-Near-Right
     0.2f,  0.2f, -0.2f,  0.0f,  0.5f,  0.0f, // 14 RightSurf : Bottom-Far-Right
	 0.2f,  0.2f,  0.2f,  0.0f,  0.5f,  0.0f, // 15 RightSurf : Top-Far-Right
	-0.2f, -0.2f, -0.2f,  0.5f,  0.0f,  0.0f, // 16 FarSurf   : Far-Bottom-Left
	-0.2f,  0.2f, -0.2f,  0.5f,  0.0f,  0.0f, // 17 FarSurf   : Far-Bottom-Right
	-0.2f, -0.2f,  0.2f,  0.5f,  0.0f,  0.0f, // 18 FarSurf   : Far-Top-Left
	-0.2f,  0.2f,  0.2f,  0.5f,  0.0f,  0.0f, // 19 FarSurf   : Far-Top-Right
 	 0.2f, -0.2f, -0.2f,  0.5f,  0.0f,  0.0f, // 20 NearSurf  : Near-Bottom-Left
     0.2f,  0.2f, -0.2f,  0.5f,  0.0f,  0.0f, // 21 NearSurf  : Near-Bottom-Right
     0.2f, -0.2f,  0.2f,  0.5f,  0.0f,  0.0f, // 22 NearSurf  : Near-Top-Left
	 0.2f,  0.2f,  0.2f,  0.5f,  0.0f,  0.0f  // 23 NearSurf  : Near-Top-Right
};

const GLuint cubeIndices[] = {
	 0,  1,  2,
	 1,  2,  3, // TopSurf
	 4,  5,  6,
	 5,  6,  7, // BotSurf
	 8,  9, 10,
	 9, 10, 11, // LeftSurf
	12, 13, 14,
	13, 14, 15, // RightSurf
	16, 17, 18,
	17, 18, 19, // FarSurf
	20, 21, 22, 
	21, 22, 23  // NearSurf
};  
	
GLuint initShaders(void)
{
	GLint success;
	GLchar infoLog[512];
	
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	
	if(!success) {
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
}

void graphicsExit(void)
{
	glDeleteVertexArrays(1, &cubeEBO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
}

void fillGLfloatArrayFromMat4(struct mat4 * inputMatrix, GLfloat outputArray[16])
{
	for (int i = 0; i < 16; i++)
		outputArray[i] = ((double*) inputMatrix)[i];
}

void graphicsConfigSetDefault(struct graphicsConfig * graCon)
{
	mat4Eye(&graCon->matSerial);
	graCon->scaleX = SCALE_DEF_X;
	graCon->scaleY = SCALE_DEF_Y;
	graCon->scaleZ = SCALE_DEF_Z;
	graCon->camDist = CAMDIST_DEF;
	graCon->projNear = PROJ_DEF_NEAR;
	graCon->projFar = PROJ_DEF_FAR;
	graCon->projWidth = PROJ_DEF_WIDTH;
	graCon->projHeight = PROJ_DEF_HEIGHT;
}

void graphicsDrawCube(struct graphicsConfig * graCon)
{		
	struct mat4 matProj;
	struct mat4 matScale;
	struct mat4 matCam;
	struct mat4 matModel;

	mat4Perspective(&matProj, graCon->projNear, graCon->projFar, graCon->projWidth, graCon->projHeight);
	mat4Eye(&matScale);
	mat4Eye(&matCam);

	mat4SetScaling(&matScale, graCon->scaleX, graCon->scaleY, graCon->scaleZ);
	mat4Mult(&graCon->matSerial, &matScale, &matModel);

	mat4SetTranslation(&matCam, 0, 0, graCon->camDist);
	mat4Mult(&matProj, &matCam, &matProj);	


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat arrayModel[16];
	GLfloat arrayProj[16];
	fillGLfloatArrayFromMat4(&matModel, arrayModel);
	fillGLfloatArrayFromMat4(&matProj, arrayProj);
	glUseProgram(cubeShader);
	glUniformMatrix4fv(cubeModelUni, 1, GL_TRUE, arrayModel);
	glUniformMatrix4fv(cubeProjUni, 1, GL_TRUE, arrayProj);
	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(*cubeIndices), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

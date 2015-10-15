#include <stdio.h>
#include <stdlib.h>  // RRRRRRRRRRRRRRRRRRRRRRRRRRRREMOVE
#include <math.h>
#include "headers/matrix4.h"



void mat4Copy(struct mat4* outputMatrix, struct mat4* inputMatrix)
{
	for (int r = 0; r < 4; r++)
		for (int c = 0; c < 4; c++)
			outputMatrix->data[r][c] = inputMatrix->data[r][c];
}

void mat4Mult(struct mat4* A, struct mat4* B, struct mat4* outputMatrix) 
{
	struct mat4 tmpMatrix = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	for (int rA = 0; rA < 4; rA++)
		for (int cB = 0; cB < 4; cB++)
			for (int cA = 0; cA < 4; cA++)
				tmpMatrix.data[rA][cB] += A->data[rA][cA] * B->data[cA][cB];

	mat4Copy(outputMatrix, &tmpMatrix);
}

void mat4SetRow(int r, struct mat4* outputMatrix, double c0, double c1, double c2, double c3)
{
	outputMatrix->data[r][0] = c0;
	outputMatrix->data[r][1] = c1;
	outputMatrix->data[r][2] = c2;
	outputMatrix->data[r][3] = c3;	
}

void mat4Perspective(struct mat4* outputMatrix, double near, double far, double width, double height)
{
	// Width and height are the dimensions of an object filling the entire viewport in the near field.
	mat4SetRow(0, outputMatrix, 2*near/width, 0, 0, 0);
	mat4SetRow(1, outputMatrix, 0, 2*near/height, 0, 0);
	mat4SetRow(2, outputMatrix, 0, 0, (far+near)/(near-far), -2*far*near/(far-near));
	mat4SetRow(3, outputMatrix, 0, 0, -1, 0);
}

void mat4Disp(struct mat4* inputMatrix, char const *const text)
{	
	printf("%s", text);
	printf("\n");
	for (int i = 1; i <= 80; i++)
		printf("_");
		
	for (int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++){
			printf("%f", inputMatrix->data[i][j]);
			printf(", ");
		}
		printf("\n");
	}
	
	for (int i = 1; i <= 80; i++)
		printf("_");
	printf("\n");
}

void mat4SetScaling(struct mat4* outputMatrix, double x, double y, double z)
{	
	outputMatrix->data[0][0] = x;
	outputMatrix->data[1][1] = y;
	outputMatrix->data[2][2] = z;
}

void mat4Init(struct mat4* outputMatrix)
{	
	mat4SetRow(0, outputMatrix, 1, 0, 0, 0);
	mat4SetRow(1, outputMatrix, 0, 1, 0, 0);
	mat4SetRow(2, outputMatrix, 0, 0, 1, 0);
	mat4SetRow(3, outputMatrix, 0, 0, 0, 1);
}

void mat4SetTranslation(struct mat4* outputMatrix, double x, double y, double z)
{
	outputMatrix->data[0][3] = x;
	outputMatrix->data[1][3] = y;
	outputMatrix->data[2][3] = z;
}

void mat4SetRotFromMat(struct mat4* outputMatrix, struct mat4* rotMatrix)
{	
	/*
	outputMatrix->data[0][0] = rotMatrix->data[0][1]; 
	outputMatrix->data[1][0] = rotMatrix->data[1][1]; 
	outputMatrix->data[2][0] = rotMatrix->data[2][1]; 
	
	outputMatrix->data[0][1] = rotMatrix->data[0][2]; 
	outputMatrix->data[1][1] = rotMatrix->data[1][2]; 
	outputMatrix->data[2][1] = rotMatrix->data[2][2]; 
	
	outputMatrix->data[0][2] = rotMatrix->data[0][0]; 
	outputMatrix->data[1][2] = rotMatrix->data[1][0]; 
	outputMatrix->data[2][2] = rotMatrix->data[2][0]; 
	*/
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			outputMatrix->data[r][c] = rotMatrix->data[r][c]; 
}


void mat4SetRotFromHPR(struct mat4* outputMatrix, double heading, double pitch, double roll)
{
	// Rmeember Z and Y are switched in relation to real world coordinates. Y is up! 
	
	struct mat4 headingMatrix;
	struct mat4 pitchMatrix;
	struct mat4 rollMatrix;
	
	mat4SetRow(0, &headingMatrix, cos(heading), -sin(heading), 0, 0);
	mat4SetRow(1, &headingMatrix, sin(heading), cos(heading), 0, 0);
	mat4SetRow(2, &headingMatrix, 0, 0, 1, 0);
	//mat4SetRow(3, rollMatrix, 0, 0, 0, 0);
	
	mat4SetRow(0, &pitchMatrix, cos(pitch), 0, -sin(pitch), 0);
	mat4SetRow(1, &pitchMatrix, 0, 1, 0, 0);
	mat4SetRow(2, &pitchMatrix, sin(pitch), 0, cos(pitch), 0);
	//mat4SetRow(3, headingMatrix, 0, 0, 0, 0);
	
	mat4SetRow(0, &rollMatrix, 1, 0, 0, 0);
	mat4SetRow(1, &rollMatrix, 0, cos(roll), sin(roll), 0);
	mat4SetRow(2, &rollMatrix, 0, -sin(roll), cos(roll), 0);
	//mat4SetRow(3, pitchMatrix, 0, 0, 0, 0};
	
	struct mat4 rotMatrix;
	mat4Mult(&headingMatrix, &pitchMatrix, &rotMatrix);
	mat4Mult(&rotMatrix, &rollMatrix, &rotMatrix);
	mat4SetRotFromMat(outputMatrix, &rotMatrix);
}
#include <stdio.h>
#include <math.h>
#include "headers/matrix4.h"

void mat4SetRowMan(int row, struct mat4 * mat, double a, double b, double c, double d)
{
	mat->data[row][0] = a;
	mat->data[row][1] = b;
	mat->data[row][2] = c;
	mat->data[row][3] = d;
}

void mat4Zero(struct mat4 * mat)
{
	for (int i = 0; i < 16; i++)
		*((double*)mat->data + i) = 0;	
}

void mat4Eye(struct mat4 * mat)
{	
	mat4SetRowMan(0, mat, 1, 0, 0, 0);
	mat4SetRowMan(1, mat, 0, 1, 0, 0);
	mat4SetRowMan(2, mat, 0, 0, 1, 0);
	mat4SetRowMan(3, mat, 0, 0, 0, 1);
}

void mat4Mult(struct mat4 * matA, struct mat4 * matB, struct mat4 * matOut)
{	
	struct mat4 matTmp;
	mat4Zero(&matTmp);
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				matTmp.data[i][j] += matA->data[i][k] * matB->data[k][j];
	*matOut = matTmp;
}

void mat4Perspective(struct mat4 * mat, double near, double far, double width, double height)
{
	// Width and height are the dimensions of an object filling the entire viewport in the near field
	mat4SetRowMan(0, mat, 2 * near/width, 0, 0, 0);
	mat4SetRowMan(1, mat, 0, 2 * near/height, 0, 0);
	mat4SetRowMan(2, mat, 0, 0, (far + near) / (near - far), -2 * far * near / (far - near));
	mat4SetRowMan(3, mat, 0, 0, -1, 0);
}

void mat4Disp(struct mat4 * mat, char const * const text)
{	
	printf("%s", text);
	printf("\n");
	for (int i = 1; i <= 80; i++)
		printf("_");
	printf("\n");
	for (int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++){
			printf("%f", mat->data[i][j]);
			printf(", ");
		}
		printf("\n");
	}
	
	for (int i = 1; i <= 80; i++)
		printf("_");
	printf("\n");
}

void mat4SetScaling(struct mat4 * mat, double x, double y, double z)
{	
	mat->data[0][0] = x;
	mat->data[1][1] = y;
	mat->data[2][2] = z;
}

void mat4SetTranslation(struct mat4 * mat, double x, double y, double z)
{
	mat->data[0][3] = x;
	mat->data[1][3] = y;
	mat->data[2][3] = z;
}

void mat4SetRotFromHPR(struct mat4 * mat, double heading, double pitch, double roll)
{
	// Rmeember Z and Y are switched in relation to real world coordinates. Y is up! 
	
	struct mat4 matHead;
	struct mat4 matPitch;
	struct mat4 matRoll;
	
	mat4SetRowMan(0, &matHead, cos(heading), -sin(heading), 0, 0);
	mat4SetRowMan(1, &matHead, sin(heading), cos(heading), 0, 0);
	mat4SetRowMan(2, &matHead, 0, 0, 1, 0);
	mat4SetRowMan(3, &matHead, 0, 0, 0, 0);
	
	mat4SetRowMan(0, &matPitch, cos(pitch), 0, sin(pitch), 0);
	mat4SetRowMan(1, &matPitch, 0, 1, 0, 0);
	mat4SetRowMan(2, &matPitch, -sin(pitch), 0, cos(pitch), 0);
	mat4SetRowMan(3, &matPitch, 0, 0, 0, 0);
	
	mat4SetRowMan(0, &matRoll, 1, 0, 0, 0);
	mat4SetRowMan(1, &matRoll, 0, cos(roll), -sin(roll), 0);
	mat4SetRowMan(2, &matRoll, 0, sin(roll), cos(roll), 0);
	mat4SetRowMan(3, &matRoll, 0, 0, 0, 0);
	
	struct mat4 matRot;
	mat4Mult(&matHead, &matPitch, &matRot);
	mat4Mult(&matRot, &matRoll, &matRot);
	*mat = matRot;
}

double mat4Det(struct mat4 * mat)
{
	double result = 0;

	result += mat->data[0][0] * mat->data[1][1] * mat->data[2][2];
	result += mat->data[0][1] * mat->data[1][2] * mat->data[2][0];
	result += mat->data[0][2] * mat->data[1][0] * mat->data[2][1];
	result -= mat->data[0][1] * mat->data[1][0] * mat->data[2][2];
	result -= mat->data[0][0] * mat->data[1][2] * mat->data[2][1];
	result -= mat->data[0][2] * mat->data[1][1] * mat->data[2][0];	

	return result;
}

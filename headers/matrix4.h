#ifndef MAT4_H
#define MAT4_H

struct mat4 
{
	double data[4][4];
};

void mat4Copy(struct mat4* outputMatrix, struct mat4* inputMatrix);
void mat4SetRow(int R, struct mat4* outputMatrix, double C0, double C1, double C2, double C3);
void mat4Perspective(struct mat4* outputMatrix, double near, double far, double width, double height);
void mat4Mult(struct mat4* A, struct mat4* B, struct mat4* outputMatrix);
void mat4Disp(struct mat4* inputMatrix, char const *const text);
void mat4SetTranslation(struct mat4* outputMatrix, double x, double y, double z);
void mat4SetRotFromMat(struct mat4* outputMatrix, struct mat4* rotMatrix);
void mat4Init(struct mat4* outputMatrix);
void mat4SetRotFromHPR(struct mat4* outputMatrix, double heading, double pitch, double roll);
void mat4SetScaling(struct mat4* outputMatrix, double x, double y, double z);

#endif /* MAT4_H */
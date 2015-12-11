#ifndef MAT4_H
#define MAT4_H

struct mat4 {
	double data[4][4];
};

void mat4SetRowMan(int row, struct mat4 * mat, double a, double b, double c, double d);
void mat4Zero(struct mat4 * mat);
void mat4Eye(struct mat4 * mat);
void mat4Mult(struct mat4 * matA, struct mat4 * matB, struct mat4 * matOut);
void mat4Perspective(struct mat4 * mat, double near, double far, double width, double height);
void mat4Disp(struct mat4 * mat, char const * const text);
void mat4SetScaling(struct mat4 * mat, double x, double y, double z);
void mat4SetTranslation(struct mat4 * mat, double x, double y, double z);
void mat4SetRotFromHPR(struct mat4 * mat, double heading, double pitch, double roll);
double mat4Det(struct mat4 * mat);
#endif /* MAT4_H */

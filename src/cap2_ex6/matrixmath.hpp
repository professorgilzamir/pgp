#pragma once

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif


void identity(GLfloat b[16]) {
	b[0] = 1; b[1] = 0; b[2] = 0; b[3] = 0;
	b[4] = 0; b[5] = 1; b[6] = 0; b[7] = 0;
	b[8] = 0; b[9] = 0; b[10] = 1; b[11] = 0;
	b[12]= 0; b[13] = 0; b[14] = 0; b[15] = 1;
}


void multMatrix4(GLfloat b[16], GLfloat a[16], GLfloat *p) {
	if (p == NULL) {
		p = new GLfloat[16];
	}
	GLfloat p0 = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
	GLfloat p1 = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
	GLfloat p2 = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
	GLfloat p3 = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];

	GLfloat p4 = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
	GLfloat p5 = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
	GLfloat p6 = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
	GLfloat p7 = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];

	GLfloat p8 = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
	GLfloat p9 = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
	GLfloat p10 = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
	GLfloat p11 = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];

	GLfloat p12 = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
	GLfloat p13 = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
	GLfloat p14 = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
	GLfloat p15 = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];

	p[0] = p0; p[1] = p1; p[2] = p2; p[3] = p3; p[4] = p4, p[5] = p5; p[6] = p6;
	p[7] = p7; p[8] = p8; p[9] = p9; p[10] = p10; p[11] = p11; p[12] = p12;
	p[13] = p13; p[14] = p14; p[15] = p15;
}


void transform(GLfloat matrix[16], GLfloat vertex[4], GLfloat *out) {
	if (out == NULL) {
		out = new float[4];
	}
	GLfloat out0 = matrix[0] * vertex[0] + matrix[1] * vertex[1] + matrix[2] * vertex[2] + matrix[3] * vertex[3];
	GLfloat out1 = matrix[4] * vertex[0] + matrix[5] * vertex[1] + matrix[6] * vertex[2] + matrix[7] * vertex[3];
	GLfloat out2 = matrix[8] * vertex[0] + matrix[9] * vertex[1] + matrix[10] * vertex[2] + matrix[11] * vertex[3];
	GLfloat out3 = matrix[12] * vertex[0] + matrix[13] * vertex[1] + matrix[14] * vertex[2] + matrix[15] * vertex[3];	

	out[0] = out0;
	out[1] = out1;
	out[2] = out2;
	out[3] = out3;
}


void insertVec3(GLfloat vertices[], GLuint pos, GLfloat vec[3]){
	GLuint fpos = 3 * pos;
	vertices[fpos] = vec[0];
	vertices[fpos] = vec[1];
	vertices[fpos] = vec[2];
}


#pragma once

#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif


namespace matrixmath {

	const GLfloat TO_RADIANS = 0.017453292519943295f;

	GLfloat* identity(GLfloat *b) {
		if (b == NULL) {
			b = new float[16];
		}
		b[0] = 1; b[1] = 0; b[2] = 0; b[3] = 0;
		b[4] = 0; b[5] = 1; b[6] = 0; b[7] = 0;
		b[8] = 0; b[9] = 0; b[10] = 1; b[11] = 0;
		b[12]= 0; b[13] = 0; b[14] = 0; b[15] = 1;
		return b;
	}

	GLfloat* ortho(GLfloat *m, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat n, GLfloat f) {
		if (m == NULL) {
			m = new GLfloat[16];
		}
		identity(m);

		GLfloat xden = right-left;
		GLfloat xnum = right+left;
		GLfloat yden = top - bottom;
		GLfloat ynum = top + bottom;
		GLfloat zden = f - n;
		GLfloat znum = f + n;

		m[0] = 2.0f/xden;
		m[3] = -xnum/xden;

		m[5] = 2.0f/yden;
		m[7] = -ynum/yden;

		m[10] = -2.0f/znum;
		m[11] = -znum/zden;
		return m;
	}


	GLfloat* perspective(GLfloat *m, GLfloat fovy, GLfloat aspect, GLfloat n, GLfloat f) {
		if (m == NULL) {
			m = new GLfloat[16];
		}
		identity(m);

		float t = 1.0f/tan(TO_RADIANS * fovy/2.0f);
		float a = aspect;

		m[0] = t/a;
		m[5] = t;
		m[10] = (f+n)/(n-f);
		m[11] = (2*n*f)/(n-f);
		m[14] = -1;
		m[15] = 0.0f;
		return m;
	}

	GLfloat* scaleMatrix4(GLfloat *m, GLfloat x, GLfloat y, GLfloat z) {
		if (m == NULL) {
			m = new GLfloat[16];
		}
		identity(m);
		m[0] = x;
		m[5] = y;
		m[10] = z;
		return m;
	}

	GLfloat* translationMatrix4(GLfloat *m, GLfloat x, GLfloat y, GLfloat z) {
		if (m == NULL) {
			m = new GLfloat[16];
		}
		identity(m);
		m[3] = x;
		m[7] = y;
		m[11] = z;
		return m;
	}

	GLfloat* rotationXMatrix4(GLfloat *m, GLfloat ang) {
		if (m == NULL) {
			m = new GLfloat[16];
		}
		GLfloat rang = TO_RADIANS * ang;

		identity(m);

		m[5] = cos(rang);
		m[6] = -sin(rang);
		m[9] = sin(rang);
		m[10] = cos(rang);
		return m;
	}

	GLfloat* rotationYMatrix4(GLfloat *m, GLfloat ang) {
		if (m == NULL) {
			m = new GLfloat[16];
		}
		GLfloat rang = TO_RADIANS * ang;

		identity(m);

		m[0] = cos(rang);
		m[2] = sin(rang);
		m[8] = -sin(rang);
		m[10] = cos(rang);
		return m;
	}

	GLfloat* rotationZMatrix4(GLfloat *m, GLfloat ang) {
		if (m == NULL) {
			m = new GLfloat[16];
		}
		GLfloat rang = TO_RADIANS * ang;

		identity(m);

		m[0] = cos(rang);
		m[1] = -sin(rang);
		m[4] = sin(rang);
		m[5] = cos(rang);
		return m;
	}

	GLfloat* multMatrix4(GLfloat b[16], GLfloat a[16], GLfloat *p) {
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
		return p;
	}


	GLfloat* transform(GLfloat matrix[16], GLfloat vertex[4], GLfloat *out) {
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
		return out;
	}


	void __insertVec3(GLfloat vertices[], GLuint pos, GLfloat vec[3]){
		GLuint fpos = 3 * pos;
		vertices[fpos] = vec[0];
		vertices[fpos] = vec[1];
		vertices[fpos] = vec[2];
	}

}

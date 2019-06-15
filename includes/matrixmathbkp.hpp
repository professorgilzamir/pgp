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

	GLfloat* scaleMatrix4(GLfloat *t, GLfloat x, GLfloat y, GLfloat z) {
		if (t == NULL) {
			t = new GLfloat[16];
			identity(t);
		}
		GLfloat m[16];
		identity(m);
		m[0] = x;
		m[5] = y;
		m[10] = z;
		return multMatrix4(m, t, t);
	}

	GLfloat* translationMatrix4(GLfloat *t, GLfloat x, GLfloat y, GLfloat z) {
		if (t == NULL) {
			t = new GLfloat[16];
			identity(t);
		}
		GLfloat m[16];
		identity(m);
		m[3] = x;
		m[7] = y;
		m[11] = z;
		return multMatrix4(m, t, t);
	}

	GLfloat* rotationXMatrix4(GLfloat *t, GLfloat ang) {
		if (t == NULL) {
			t = new GLfloat[16];
			identity(t);
		}

		GLfloat m[16];
		identity(m);

		GLfloat rang = TO_RADIANS * ang;

		identity(m);

		m[5] = cos(rang);
		m[6] = -sin(rang);
		m[9] = sin(rang);
		m[10] = cos(rang);
		multMatrix4(m, t, t);
		return t;
	}

	GLfloat* rotationYMatrix4(GLfloat *t, GLfloat ang) {
		if (t == NULL) {
			t = new GLfloat[16];
			identity(t);
		}

		GLfloat m[16];
		identity(m);

		GLfloat rang = TO_RADIANS * ang;

		identity(m);

		m[0] = cos(rang);
		m[2] = sin(rang);
		m[8] = -sin(rang);
		m[10] = cos(rang);
		multMatrix4(m, t, t);
		return t;
	}

	GLfloat* rotationZMatrix4(GLfloat *t, GLfloat ang) {
		if (t == NULL) {
			t = new GLfloat[16];
			identity(t);
		}	

		GLfloat m[16];
		identity(m);

		GLfloat rang = TO_RADIANS * ang;

		identity(m);

		m[0] = cos(rang);
		m[1] = -sin(rang);
		m[4] = sin(rang);
		m[5] = cos(rang);
		multMatrix4(m, t, t);
		return t;
	}

	GLfloat* transposeMatrix4(GLfloat a[16], GLfloat *p) {
		if (p == NULL) {
			p = new GLfloat[16];
		}

		GLfloat p0 = a[0];
		GLfloat p1 = a[4];
		GLfloat p2 = a[8];
		GLfloat p3 = a[12];

		GLfloat p4 = a[1];
		GLfloat p5 = a[5];
		GLfloat p6 = a[9];
		GLfloat p7 = a[13];

		GLfloat p8 = a[2];
		GLfloat p9 = a[6];
		GLfloat p10 = a[10];
		GLfloat p11 = a[14];

		GLfloat p12 = a[3];
		GLfloat p13 = a[7];
		GLfloat p14 = a[11];
		GLfloat p15 = a[15];

		p[0] = p0; p[1] = p1; p[2] = p2; p[3] = p3; p[4] = p4, p[5] = p5; p[6] = p6;
		p[7] = p7; p[8] = p8; p[9] = p9; p[10] = p10; p[11] = p11; p[12] = p12;
		p[13] = p13; p[14] = p14; p[15] = p15;
		return p;
	}

	GLfloat* transposeMatrix3(GLfloat a[9], GLfloat *p) {
		if (p == NULL) {
			p = new GLfloat[9];
		}

		GLfloat p0 = a[0];
		GLfloat p1 = a[3];
		GLfloat p2 = a[6];

		GLfloat p3 = a[1];
		GLfloat p4 = a[4];
		GLfloat p5 = a[7];

		GLfloat p6 = a[2];
		GLfloat p7 = a[5];
		GLfloat p8 = a[8];

		p[0] = p0; p[1] = p1; p[2] = p2; p[3] = p3; p[4] = p4, p[5] = p5; p[6] = p6;
		p[7] = p7; p[8] = p8;
		return p;
	}


	GLfloat det(GLfloat m[9]) {
		return m[6] * m[1] * m[5] + m[0] * m[4] * m[8] + m[3] * m[7] * m[2] 
		       - m[0] * m[7] * m[5] - m[6] * m[4] * m[2] - m[3] * m[1] * m[8];
	}

	bool inverse(GLfloat m[9], GLfloat out[9], GLfloat precision=0.000000001) {
		GLfloat d = det(m);
		if ( abs(d) < precision ) return false;

		GLfloat inv = 1.0f/d;
		printf("invdet=%f\n",inv);

		GLfloat a0 = (m[4] * m[8] - m[5] * m[7]);
		GLfloat a3 = (m[3] * m[8] - m[5] * m[6]) * -1;
		GLfloat a6 = (m[3] * m[7] - m[4] * m[6]);
		GLfloat a1 = (m[1] * m[8] - m[7] * m[2]) * -1;
		GLfloat a4 = (m[0] * m[8] - m[6] * m[2]);
		GLfloat a7 = (m[0] * m[7] - m[6] * m[1]) * -1;
		GLfloat a2 = (m[1] * m[5] - m[4] * m[2]);
		GLfloat a5 = (m[0] * m[5] - m[3] * m[2]) * -1;
		GLfloat a8 = (m[0] * m[4] - m[3] * m[1]);

		out[0] = a0 * inv; out[1] = a1 * inv; out[2] = a2 * inv; out[3] = a3 * inv;
		out[4] = a4 * inv; out[5] = a5 * inv; out[6] = a6 * inv; out[7] = a7 * inv;
		out[8] = a8 * inv;

		return true;
	}


	void subMatrix3FromMatrix4(GLfloat matrix[16], GLfloat submatrix[9]){
		submatrix[0] = matrix[0];
		submatrix[1] = matrix[1];
		submatrix[2] = matrix[2];

		submatrix[3] = matrix[4];
		submatrix[4] = matrix[5];
		submatrix[5] = matrix[6];

		submatrix[6] = matrix[8];
		submatrix[7] = matrix[9];
		submatrix[8] = matrix[10];
	}

	void copySubMatrixInMatrix4(GLfloat matrix[16], GLfloat submatrix[9]) {
		matrix[0] = submatrix[0];
		matrix[1] = submatrix[1];
		matrix[2] = submatrix[2];

		matrix[4] = submatrix[3];
		matrix[5] = submatrix[4];
		matrix[6] = submatrix[5];

		matrix[8] = submatrix[6];
		matrix[9] = submatrix[7];
		matrix[10] = submatrix[8];	
	}


	GLfloat* transform3(GLfloat matrix[9], GLfloat vertex[3], GLfloat *out) {
		if (out == NULL) {
			out = new float[3];
		}
		GLfloat out0 = matrix[0] * vertex[0] + matrix[1] * vertex[1] + matrix[2] * vertex[2];
		GLfloat out1 = matrix[3] * vertex[0] + matrix[4] * vertex[1] + matrix[5] * vertex[2];
		GLfloat out2 = matrix[6] * vertex[0] + matrix[7] * vertex[1] + matrix[8] * vertex[2];
		
		out[0] = out0;
		out[1] = out1;
		out[2] = out2;

		return out;
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

	GLfloat* getViewInverse(GLfloat matrix[16], GLfloat *out){ 
			if (out == NULL) {
				out = new float[16];
			}
			identity(out);
			GLfloat submatrix[9];
			subMatrix3FromMatrix4(matrix, submatrix);
			transposeMatrix3(submatrix, submatrix);
			copySubMatrixInMatrix4(out, submatrix);

			GLfloat v[3];

			v[0] = matrix[3];
			v[1] = matrix[7];
			v[2] = matrix[11];

			transform3(submatrix, v, v);

			out[3] = -v[0];
			out[7] = -v[1];
			out[11] = -v[2];
			out[12] = 0.0f;
			out[13] = 0.0f;
			out[14] = 0.0f;
			out[15] = 1.0f;
			return out;
	}

	GLfloat* getProjectionInverse(GLfloat matrix[16], GLfloat *out){
			if (out == NULL) {
				out = new float[16];
			}
			identity(out);

			out[0] = 1.0f/matrix[0];
			out[5] = 1.0f/matrix[5];
			out[10] = 0.0f;
			out[11] = 1.0f/matrix[14]; 
			out[14] = 1.0f/matrix[11];
			out[15] = -matrix[10]/(matrix[11] * matrix[14]);
			return out;
	}
}

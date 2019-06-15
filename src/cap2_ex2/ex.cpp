/*
		Este exemplo mostra o uso básico de programa que usa
		OpenGL e GLSL para mostrar um triangulo na tela.
*/
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cmath>
#include <iostream>

#include "shaderutils.hpp"
#include "matrixmath.hpp"

/* Usando o GLUT com gerenciador de janelas */
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/glut.h>
#endif

using namespace std;
using namespace shaderutils;
using namespace matrixmath;

GLfloat triangulo_vertices[] = {
	0.0, 0.8,
	-0.8, -0.8,
	0.8, -0.8
};

GLuint triangulo_indices[] = {
	0, 1, 2
};

GLfloat transformacao[] = {
	1.0, 0.0,
	0.0, 1.0
};

GLuint program;
GLfloat alfa = 0.0f;
GLuint qtdCoordenadas = 2;
ShaderProxy *proxy;

void atualizarDesenho();

bool inTriangle(float t[], float x, float y){
	float u[3], v[3], w[3], k[3], l[3];
	u[0] = t[2] - t[0];
	u[1] = t[3] - t[1];
	u[2] = 0.0f;
	v[0] = t[4] - t[2];
	v[1] = t[5] - t[3];
	v[2] = 0.0;
	w[0] = t[0] - t[4];
	w[1] = t[1] - t[5];
	w[2] = 0.0;

	float rightside = 1;

	k[0] = x - t[0];
	k[1] = y - t[1];
	k[2] = 0;
	crossproduct(u, k, l);
	rightside = l[2];
	k[0] = x - t[2];
	k[1] = y - t[3]; 
	crossproduct(v, k, l);
	if (rightside * l[2] <  0) {
		return false;
	}
	rightside = l[2];
	k[0] = x - t[4];
	k[1] = y - t[5];
	crossproduct(w, k, l);
	if (rightside * l[2] < 0) {
		return false;
	}
	return true;
}


void onMouseClick(GLint buttom, GLint state, GLint x, GLint y){

	if (state == GLUT_DOWN){
		return;
	}
	
	float f[2];
	f[0] = 2.0f * x/640.0f - 1.0f;
	f[1] = 1.0 -  2.0f*y/480.0f;


	GLfloat tt[4];
	inverse2(transformacao, tt);
	transform2(tt, f, f);

	if (inTriangle(triangulo_vertices, f[0], f[1]) ) {
		alfa += 0.01;
		transformacao[0] = cos(alfa);
		transformacao[1] = -sin(alfa);
		transformacao[2] = sin(alfa);
		transformacao[3] = cos(alfa);
		cout<<"DENTRO"<<endl;
	} else {
		cout<<"FORA"<<endl;
	}


	glutPostRedisplay();
}

int inicializar(void)
{
	try {
		program = genProgram("shader.vs", "shader.fs");
		proxy = new ShaderProxy(program);
	} catch(string error) {
		cout<<error<<endl;
		return 0;
	}

	try {
		proxy->useProgram();

		proxy->setAttribute("vertice", triangulo_vertices, sizeof(triangulo_vertices), 
								qtdCoordenadas);
									
		proxy->setElementPrimitive(triangulo_indices, sizeof(triangulo_indices), GL_TRIANGLES);
	} catch(string error) {
		cout<<error<<endl;
		return 0;
	}
	return 1;
	return 1;
}

void atualizarDesenho()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	proxy->useProgram();
	proxy->setUniformMatrix2fv("transformacao", transformacao, 1, GL_TRUE);
	proxy->drawElements();
	glutSwapBuffers();
}

void finalizar()
{
  glDeleteProgram(program);
  if (proxy != 0) {
	  delete proxy;
	  proxy = 0;
  }
}

int main(int argc, char* argv[])
{
  /* Funções necessárias para iniciar a GLUT */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
  glutInitWindowSize(640, 480);
  glutCreateWindow("Girando um triangulo");
   #ifndef __APPLE__
	GLenum glew_status = glewInit();
  	if (glew_status != GLEW_OK)
  	{
    		fprintf(stderr, "Erro: %s\n", glewGetErrorString(glew_status));
    		return EXIT_FAILURE;
  	}
  #endif
  /* Quando as funções de inicialização são executadas sem erros,
  o programa pode iniciar os recursos */
  if (1 == inicializar())
  {
    /* Pode então mostrar se tudo correr bem */
    glutDisplayFunc(atualizarDesenho);
    glutMouseFunc(onMouseClick);
    glutMainLoop();
  }

  /* Se o ocasionalmente programa sair ,
  liberamos os recursos da memória e completaremos ele com sucesso*/
  finalizar();
  return EXIT_SUCCESS;
}

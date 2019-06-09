/* Usando o padrão de saída fprintf */
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
/* Usando o GLUT com gerenciador de janelas */

#include "matrixmath.hpp"
#include "shaderutils.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif

using namespace std;
using namespace matrixmath;
using namespace shaderutils;

GLfloat rot = 0.0f;
GLfloat rotInc = 11.0f;

ShaderProxy *proxy = 0;

GLfloat cube_vertices[] = {
	 1, -1,   1, //0
	 1,  1,   1, //1
	-1,  1,   1, //2
	-1, -1,   1, //3
	-1, -1,  -1, //4
	-1,  1,  -1, //5
	 1,  1,  -1, //6
	 1, -1,  -1, //7
};

GLfloat cube_colors[] = {
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1
};

GLuint cube_indices[] = {
		0, 1, 2, 3, //Face Fontal
		4, 5, 6, 7, //Face Traseira
		1, 6, 5, 2, //Face de Cima
		0, 3, 4, 7, //Face de Baixo
		2, 5, 4, 3, //Face da Esquerda
		7, 6, 1, 0, //Face da Direita 
};


GLfloat scale[16];
GLfloat translation[16];
GLfloat proj[16];
GLfloat rotx45[16];
GLfloat matrix[16];
GLuint program;

int mainwindow=-1;
GLuint width = 640, height = 480;


void initializeMatrix() {
	//ortho(proj, -100, 100, -100, 100, 0.0001, 1000.0);
	identity(proj);
	identity(matrix);
	identity(rotx45);
	identity(translation);
	identity(scale);
	GLfloat aspect = (GLfloat)width/height;
	perspective(proj, 45.0f, aspect, 0.01f, 1000.0f);
	scaleMatrix4(scale,  20, 20, 20);
	translationMatrix4(translation, 0, 0, -20.01f);
}

void updateMatrix() {
	identity(matrix);
	rotationXMatrix4(rotx45, rot);
	multMatrix4(matrix, proj, matrix);
	multMatrix4(matrix, translation, matrix);
	multMatrix4(matrix, scale, matrix);
	multMatrix4(matrix, rotx45, matrix);
	rot = rot + rotInc;
}


/* COLOCAREMOS AS VARIAVEIS GLOBAIS AQUI MAIS TARDE */

int inicializar(void)
{

	try {
		program = genProgram("shader.vs", "shader.fs");
	} catch(string error) {
		cout<<error<<endl;

		return 0;
	}

	const GLuint QTD_ATRIBUTOS = 2;

	proxy = new ShaderProxy(program, QTD_ATRIBUTOS);
	
	try {
		proxy->useProgram();
		cout<<sizeof(cube_vertices)<<endl;
		proxy->setAttribute("coord3d", cube_vertices, sizeof(cube_vertices));
		proxy->setAttribute("color3d", cube_colors, sizeof(cube_colors));
		proxy->setElementPrimitive(cube_indices, sizeof(cube_indices));
		proxy->setUniformMatrix4fv("matrix", matrix, 1, GL_TRUE);
	} catch(string error) {
		cout<<error<<endl;
		return 0;
	}

	initializeMatrix();
	updateMatrix();
	return 1;
}

void atualizarDesenho()
{

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	proxy->useProgram();
	proxy->setUniformMatrix4fv("matrix", matrix, 1, GL_TRUE);
	proxy->setUniformMatrix4fv("matrix", matrix, 1, GL_TRUE);
	proxy->drawElements();
	glutSwapBuffers();
}

void finalizar()
{
  glDeleteProgram(program);
  if (!proxy) {
  	delete proxy;
  	proxy = 0;
  }

}

void fecharJanela() {
	glutDestroyWindow(mainwindow);
	finalizar();
	exit(0);
}


void rotate(unsigned char key, int x, int y) {
	if (key == 'r') {
		updateMatrix();
		atualizarDesenho();
	}
}

int main(int argc, char* argv[])
{
	/* Funções necessárias para iniciar a GLUT */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(width, height);
	mainwindow = glutCreateWindow("Meu Primeiro Cubo Minha Vida");

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	const char* ver = (const char*)glGetString(GL_VERSION);
	char major_number = ver[0];
	if (major_number < '2'){
		cout<<"Seu computador  nao suporta a versao 2.1 do OpenGL"<<endl;
		return 0;
	}

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
		glutWMCloseFunc(fecharJanela);
		glutDisplayFunc(atualizarDesenho);
		glutKeyboardFunc(rotate);
		glutMainLoop();
	}

	/* Se o ocasionalmente programa sair ,
	liberamos os recursos da memória e completaremos ele com sucesso*/
	return EXIT_SUCCESS;
}

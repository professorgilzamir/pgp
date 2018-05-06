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

#include "esferoide.cpp"

//PROPRIEDADES DA CAMERA

//Propriedades da projecao perspectiva da camera
GLfloat fov = 45.0; //fov = field of view ou angulo de abertura da camera
GLfloat eye_position[3]; //Posicao da camera
GLfloat eye_orientation[2]; //Orientacao da camera


//PROPRIEDADES DOS OBJETOS (MODEL) NA CENA
GLfloat scale[16]; //dimensao do objeto
GLfloat translation[16]; //deslocamento em relacao a origem da cena


//MATRIZES QUE COMBINAM AS TRANSFORMACOES DE MODELO, CAMERA E PROJECAO : MODEL-VIEW-PROJECTION
GLfloat model[16]; //define as caracteristicas geometricas do modelo
GLfloat view[16]; //define a posicao e a orientacao da camera
GLfloat projection[16]; //define a projecao da camera


GLfloat matrix[16]; //contem o produto projection * view * model


//VARIAVEIS UTILIZADAS NO MAPEAMENTO DE DADOS DO PROGRAMA PARA OS SHADERS

GLuint program; //ID do programa, lembrando que um programa aqui eh a combinacao de vertex com fragment shader


int mainwindow=-1; //ID da janela de visualizacao criada pelo GLUT
GLuint width = 640, height = 480;

ShaderProxy *proxy = 0;


/**
* 
* Configuracao inicial das matrizes MODEL-VIEW-PROJECTION
*
*/
void initializeMatrix() {
	//ortho(projection, -100, 100, -100, 100, 0.0001, 1000.0);
	identity(model);
	GLfloat aspect = (GLfloat)width/height;
	perspective(projection, fov, aspect, 0.01f, 10000.0f);

	scaleMatrix4(scale,  20, 20, 20);
	translationMatrix4(translation, 0, 0, -200);
	multMatrix4(model, translation, model);
	multMatrix4(model, scale, model);
}


/**
*
* Atualizacao das matrizes de acordo com as acoes do usuario
*
*/
void updateMatrix() {
	identity(matrix);
	identity(view);


	GLfloat rotViewX[16];
	GLfloat rotViewY[16];

	rotationXMatrix4(rotViewX, eye_orientation[0]);
	rotationYMatrix4(rotViewY, eye_orientation[1]);

	//EXERCICIO IMPLEMENTAR MOVIMENTO DA CÂMERA (2 PONTOS NA SEGUNDA AP)
	multMatrix4(view, rotViewY, view);
	multMatrix4(view, rotViewX, view);

	transposeMatrix4(view, view);
}


int inicializar(void)
{
	initializeMatrix();
	updateMatrix();
	try {
		program = genProgram("shader.vs", "shader.fs");
	} catch(string error) {
		cout<<error<<endl;

		return 0;
	}

	const GLuint QTD_ATRIBUTOS = 1;

	proxy = new ShaderProxy(program, QTD_ATRIBUTOS);
	
	try {
		proxy->useProgram();
		proxy->setAttribute("coord3d", esferoide_vertexList, sizeof(esferoide_vertexList));
		proxy->setElementPrimitive(esferoide_indexList, sizeof(esferoide_indexList));
		proxy->setUniformMatrix4fv("projection", projection, 1, GL_TRUE);
		proxy->setUniform4f("materialColor", 1.0f, 0.0f, 0.0f, 1.0f);
	} catch(string error) {
		cout<<error<<endl;
		return 0;
	}


	return 1;
}


/**
*
* Atualiza o desenho na tela de acordo com o modelo de cena atual.
*
*/
void atualizarDesenho()
{

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	proxy->useProgram();
	proxy->setUniformMatrix4fv("model", model, 1, GL_TRUE);
	proxy->setUniformMatrix4fv("view", view, 1, GL_TRUE);
	proxy->drawElements();
	glutSwapBuffers();
}

/**
*
* Desaloca o programa gerado.
*
*/
void finalizar()
{
  glDeleteProgram(program);
  if (!proxy) {
  	delete proxy;
  	proxy = 0;
  }
}
/**
*
* Garante que a janela da aplicacao sera fechada.
*
*/
void fecharJanela() {
	glutDestroyWindow(mainwindow);
	finalizar();
	exit(0);
}

/**
*
* Captura eventos do teclado que representam acoes do usuario.
*
*/
void eventoDeTeclado(unsigned char key, int x, int y) {
	if (key == 'i') {
		eye_orientation[0] += 10.0f;
	} else if (key == 'k') {
		eye_orientation[0] -= 10.0f;
	}

	if (key == 'j') {
		eye_orientation[1] += 10;
	} else if (key == 'l') {
		eye_orientation[1] -= 10;
	}


	updateMatrix();
	glutPostRedisplay();
}

/**
*
* Ponto de entrada do programa.
*
*/
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
		glutKeyboardFunc(eventoDeTeclado);
		glutMainLoop();
	}

	/* Se o ocasionalmente programa sair ,
	liberamos os recursos da memória e completaremos ele com sucesso*/
	return EXIT_SUCCESS;
}

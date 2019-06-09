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
#include "scene.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif

using namespace std;
using namespace matrixmath;
using namespace shaderutils;
using namespace scene;

#include "esferoide.cpp"


//VARIAVEIS UTILIZADAS NO MAPEAMENTO DE DADOS DO PROGRAMA PARA OS SHADERS

GLuint program; //ID do programa, lembrando que um programa aqui eh a combinacao de vertex com fragment shader


int mainwindow=-1; //ID da janela de visualizacao criada pelo GLUT
GLuint width = 640, height = 480;

ShaderProxy *proxy = 0;
Camera *camera = 0;
ShadedObject *object = 0;
GLfloat eye_orientation[2];
GLfloat normais[esferoide_countVertices*3];


void initializeMatrix() {
	eye_orientation[0] = 0.0f;
	eye_orientation[1] = 0.0f;

	camera->setPerspective(45.0, width/(float)height, 0.0001f, 10000.0f);
	object->data.translate(0, 0, -10);
}

void updateMatrix() {
	camera->setViewIdentity();
	camera->rotateX(eye_orientation[0]);
	camera->rotateY(eye_orientation[1]);
}


int inicializar(void)
{

	try {
		program = genProgram("shader.vs", "shader.fs");
	} catch(string error) {
		cout<<error<<endl;

		return 0;
	}

	getNormals(esferoide_countVertices, esferoide_indexList, esferoide_normalIndexList, esferoide_normalList, esferoide_countPrimitives, 
							esferoide_primitiveSize, normais);

	const GLuint QTD_ATRIBUTOS = 2;

	proxy = new ShaderProxy(program, QTD_ATRIBUTOS);
	camera = new Camera(proxy);
	object = new ShadedObject("esferoide", proxy);

	initializeMatrix();

	object->data.setDiffuseReflection(0.8f, 0.0f, 0.0f, 1.0f);
	object->data.setAmbientReflection(0.4f, 0.4f, 0.4f, 1.0f);
	object->data.setSpecularReflection(1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

	updateMatrix();
	
	try {
		proxy->useProgram();
		proxy->setAttribute("coord3d", esferoide_vertexList, sizeof(esferoide_vertexList));
		proxy->setAttribute("normal3d", normais, sizeof(normais));
		proxy->setElementPrimitive(esferoide_indexList, sizeof(esferoide_indexList));
		camera->update();
		object->update();
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
	glClearColor(0.4, 0.4, 0.4, 0.4);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	try {
		camera->update();
		object->update();
		object->drawElements();
	} catch(string error) {
		cout<<error<<endl;
	}
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
	mainwindow = glutCreateWindow("API Scene");

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

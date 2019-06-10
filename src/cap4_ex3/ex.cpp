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

GLfloat eye_orientation[2];
GLfloat normais[esferoide_countVertices*3];


std::vector< ShadedObject* > objects;

GLfloat mousefX, mousefY;
GLint mouseDX, mouseDY;

GLfloat fov = 90;

GLfloat projectioninverse[16];
GLfloat viewinverse[16];

void initializeMatrix() {
	eye_orientation[0] = 0.0f;
	eye_orientation[1] = 0.0f;
	srand(time(0));

	camera->setPerspective(fov, width/(float)height, 0.0001f, 1000.0f);

	for (int i = 0; i < 10; i++) {
		objects[i]->data.translate(rand() % 30 - 15, rand() % 10 - 5, -1 * (rand()%30) - 10 );
	}
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
	
	ShadedObject *object = new ShadedObject("esferoide", proxy);
	objects.push_back(object);

	object->data.setDiffuseReflection(0.8f, 0.0f, 0.0f, 1.0f);
	object->data.setAmbientReflection(0.4f, 0.4f, 0.4f, 1.0f);
	object->data.setSpecularReflection(1.0f, 1.0f, 1.0f, 1.0f, 100.0f);

	for (GLuint i = 0; i < 9; i++) {
		ShadedObject *so = new ShadedObject("esferoide", proxy);
		so->data.setDiffuseReflection(0.8f, 0.0f, 0.0f, 1.0f);
		so->data.setAmbientReflection(0.4f, 0.4f, 0.4f, 1.0f);
		so->data.setSpecularReflection(1.0f, 1.0f, 1.0f, 1.0f, 100.0f);
		objects.push_back(so);
	}

	initializeMatrix();
	updateMatrix();
	
	try {
		proxy->useProgram();
		proxy->setAttribute("coord3d", esferoide_vertexList, sizeof(esferoide_vertexList));
		proxy->setAttribute("normal3d", normais, sizeof(normais));
		proxy->setElementPrimitive(esferoide_indexList, sizeof(esferoide_indexList));
		camera->update();
		for (GLuint i = 0; i < 10; i++) {
			objects[i]->update();
		}
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
		for (GLuint i = 0; i < 10; i++) {
			objects[i]->update();
			objects[i]->drawElements();
		}
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





void mouseMotion(int x, int y) {
	mouseDX = x;
	mouseDY = y;

	GLfloat n[4];
	n[0] = (2.0f * x)/width - 1.0f;
	n[1] = 1.0f - (2.0f * y)/height;
	n[2] = 0.0f;
	n[3] = 1.0;


	GLfloat t1 = sqrt(n[0] * n[0] + n[1] * n[1]);
	GLfloat mindiff = 100000000000000;
	ShadedObject *minobj = 0;
	cout<<"----------------------------------------------------------------------------"<<endl;
	for (int i = 0; i < 10; i++) {
		GLfloat pos[4];
		objects[i]->data.getPosition(pos);

		transform(camera->view, pos, pos);
		transform(camera->projection, pos, pos);

		pos[0] /= pos[3];
		pos[1] /= pos[3];
		
		GLfloat t2 = sqrt(pos[0] * pos[0] + pos[1] * pos[1]);

		GLfloat diff = abs(t1 - t2);


			GLfloat cosang = 0;

		if (t1 != 0 && t2 != 0){
			cosang = (pos[0] * n[0] + pos[1] * n[1])/(t1 * t2);
		}

		cout<<abs(cosang - 1.0)<<", "<<abs(t1 - t2)<<endl;

		GLfloat coseps = 9.0e-3;
		GLfloat deps = 0.15;

		if (abs(cosang - 1.0) < coseps && diff < deps && mindiff  > diff) {
			if (minobj != 0)
				minobj->data.setDiffuseReflection(0.8f, 0.0f, 0.0f, 1.0f);
			minobj = objects[i];
			mindiff = diff;
		} else {
			objects[i]->data.setDiffuseReflection(0.8f, 0.0f, 0.0f, 1.0f);
		}
	}
	if (minobj != 0)
		minobj->data.setDiffuseReflection(0.0f, 0.8f, 0.0f, 1.0f);
	cout<<"----------------------------------------------------------------------------"<<endl;
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
		glutPassiveMotionFunc(mouseMotion);
		glutMotionFunc(mouseMotion);
		glutMainLoop();
	}

	/* Se o ocasionalmente programa sair ,
	liberamos os recursos da memória e completaremos ele com sucesso*/
	return EXIT_SUCCESS;
}

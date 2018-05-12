/* Usando o padrão de saída fprintf */
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <lodepng.h>
/* Usando o GLUT com gerenciador de janelas */

#include <matrixmath.hpp>
#include <shaderutils.hpp>
#include <scene.hpp>
#include <textureutils.hpp>

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
using namespace textureutils;

GLfloat fov = 45.0;

GLfloat eye_position[3];
GLfloat eye_orientation[2];

GLfloat cube_vertices[] = {
	 1, -1,   1, //0
	 1,  1,   1, //1
	-1,  1,   1, //2
	-1, -1,   1, //3

	-1, -1,  -1, //4
	-1,  1,  -1, //5
	 1,  1,  -1, //6
	 1, -1,  -1, //7

	 1,  1,   1, //8
	 1,  1,  -1, //9
	-1,  1,  -1, //10
	-1,  1,   1, //11

	 1, -1,   1, //12
	-1, -1,   1, //13
	-1, -1,  -1, //14
	 1, -1,  -1, //15

	-1,  1,   1, //16
	-1,  1,  -1, //17
	-1, -1,  -1, //18
	-1, -1,   1, //19

	 1, -1,  -1, //20
	 1,  1,  -1, //21
	 1,  1,   1, //22
	 1, -1,   1 //23
};

GLfloat cube_textures[] = {
	1.0f, 1.0f, 
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	0.0f, 0.0f, 
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,

	0.0f, 0.0f, 
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,

	0.0f, 0.0f, 
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,

	0.0f, 0.0f, 
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
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
		8, 9, 10, 11, //Face de Cima
		12, 13, 14, 15, //Face de Baixo
		16, 17, 18, 19, //Face da Esquerda
		20, 21, 22, 23 //Face da Direita
};

GLuint program;
ShaderProxy *proxy = 0;
ShadedObject *cubo = 0;
Camera *camera = 0;

int mainwindow=-1;

GLuint width = 640, height = 480;

void initializeMatrix() {
	camera->setPerspective(45.0, width/(float)height, 0.001f, 10000.0f);
	cubo->data.translate(0, 0, -10);
	cubo->data.scale(1.5f, 1.5f, 1.5f);
}

void updateMatrix() {
	//identity(cubo->data.transform);
	//cubo->data.translate(0,0,-10);
	camera->setViewIdentity();
	camera->translate(0,0,10);
	camera->rotateX(eye_orientation[0]);
	camera->rotateY(eye_orientation[1]);
	camera->translate(0,0,-10);
}

int inicializar(void)
{

	try {
		program = genProgram("shader.vs", "shader.fs");
	} catch(string error) {
		cout<<error<<endl;

		return 0;
	}

	const GLuint QTD_ATRIBUTOS = 3;

	proxy = new ShaderProxy(program, QTD_ATRIBUTOS);
	cubo = new ShadedObject("cubo", proxy);
	camera = new Camera(proxy);

	initializeMatrix();
	updateMatrix();

	try {
		proxy->useProgram();
		proxy->setAttribute("coord3d", cube_vertices, sizeof(cube_vertices));
		proxy->setAttribute("texcoord", cube_textures, sizeof(cube_textures), 2);
		proxy->setUniform1i("tex", 0);
		proxy->setElementPrimitive(cube_indices, sizeof(cube_indices));
	} catch(string error) {
		cout<<error<<endl;
		return 0;
	}


	std::vector<unsigned char> image;
	size_t s, t;

	try {
		image = getTextureFromImage("texture.png", s, t);
	} catch(string e) {
		cout<<e<<endl;
		return 0;
	}

	try {
		proxy->setTexture(&image[0], s, t);
		glActiveTexture(GL_TEXTURE0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		camera->update();
		cubo->update();
	} catch(string error) {
		cout<<error<<endl;
		return 0;
	}



	return 1;
}

void atualizarDesenho()
{
	glClearColor(0.6f, 0.6f, 0.6f, 0.6f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	try {
		camera->update();
		cubo->update();
		cubo->drawElements();
	} catch(string error) {
		cout<<error<<endl;
	}
	glutSwapBuffers();
}

void finalizar()
{
  glDeleteProgram(program);
  if (proxy != 0) {
  	delete proxy;
  	proxy = 0;
  }

  if (cubo != 0) {
  	delete cubo;
  	cubo = 0;
  }

  if (camera != 0) {
  	delete camera;
  	camera = 0;
  }
}

void fecharJanela() {
	glutDestroyWindow(mainwindow);
	finalizar();
	exit(0);
}


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
	atualizarDesenho();
}

int main(int argc, char* argv[])
{
	/* Funções necessárias para iniciar a GLUT */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(width, height);
	mainwindow = glutCreateWindow("Scene API: Texture in Action!");

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

/* Usando o padrão de saída fprintf */
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include "shaderutils.hpp"
/* Usando o GLUT com gerenciador de janelas */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif

using namespace std;
using namespace shaderutils;

GLfloat triangulo_vertices[] = {
	0.0, 0.8,
	-0.8, -0.8,
	0.8, -0.8
};

GLuint program;
ShaderProxy *proxy;

int mainwindow=-1;

int inicializar(void)
{
	program = genProgram("shader.vs", "shader.fs");
	proxy = new ShaderProxy(program);

	try {
		proxy->setAttribute("coord2d", triangulo_vertices, sizeof(triangulo_vertices), 2);
	} catch(string error){
		cout<<error<<endl;
		return 0;
	}

	return 1;
}

void atualizarDesenho()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	proxy->useProgram();
	proxy->drawArrays(GL_TRIANGLES);
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

void fecharJanela() {
	glutDestroyWindow(mainwindow);
	finalizar();
	exit(0);
}

int main(int argc, char* argv[])
{
  /* Funções necessárias para iniciar a GLUT */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(640, 480);
  mainwindow = glutCreateWindow("Guia do OpenGL 2.1");

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
    glutMainLoop();
  }

  /* Se o ocasionalmente programa sair ,
  liberamos os recursos da memória e completaremos ele com sucesso*/
  return EXIT_SUCCESS;
}

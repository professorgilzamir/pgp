#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>

#include "matrixmath.hpp"
#include "shaderutils.hpp"

/* Usando o GLUT com gerenciador de janelas */
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif

using namespace std;
using namespace matrixmath;
using namespace shaderutils;

GLfloat geometria[] = {
	0.0f, 0.0f,
	0.5f, 0.0f,
	0.5f, 0.5f
};

GLuint topologia[] = {
	0, 1, 2
};

const GLuint quantidadeCoordenadasPorVertice=2;

GLuint program; //ID do programa, lembrando que um programa aqui eh a combinacao de vertex com fragment shader

int mainwindow=-1; //ID da janela de visualizacao criada pelo GLUT
GLuint width = 640, height = 480;

ShaderProxy *proxy;

/**
*
* Compila os shaders, monta o programa e cria buffers iniciais
*
*/
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
		proxy->setAttribute("posicao", geometria, sizeof(geometria), 
								quantidadeCoordenadasPorVertice);
		proxy->setUniform4f("cor", 1.0f, 0.0f, 0.0f, 0.0f);
		proxy->setElementPrimitive(topologia, sizeof(topologia), GL_POINTS);
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
void atualizarTela()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_POINT_SMOOTH); //suavise as bordas do ponto
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE); //permite alterar o tamanho dos pontos no vertex shader

	if (proxy) {
		proxy->useProgram();
		try {
			proxy->drawElements();
		} catch(string error) {
			cout<<error<<endl;
		}
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
  if (proxy != 0) {
	  proxy->close();
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
* Ponto de entrada do programa.
*
*/
int main(int argc, char* argv[])
{
	/* Funções necessárias para iniciar a GLUT */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(width, height);
	mainwindow = glutCreateWindow("Meu Primeiro objeto Minha Vida");

	const char* ver = (const char*)glGetString(GL_VERSION);
	char major_number = ver[0];
	if (major_number < '2'){
		cout<<"Seu computador  nao suporta a versao 2.1 do OpenGL"<<endl;
		return 0;
	}
	
	/* Quando as funções de inicialização são executadas sem erros,
	o programa pode iniciar os recursos */
	if (1 == inicializar())
	{
		/* Pode então mostrar se tudo correr bem */
		glutWMCloseFunc(fecharJanela);
		glutDisplayFunc(atualizarTela);
		glutMainLoop();
	}

	/* Se o ocasionalmente programa sair ,
	liberamos os recursos da memória e completaremos ele com sucesso*/
	return EXIT_SUCCESS;
}

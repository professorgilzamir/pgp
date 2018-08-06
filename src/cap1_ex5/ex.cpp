/* Usando o padrão de saída fprintf */
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

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

//Onde serao armazenados os pontos que formam as linhas do objeto.
vector<GLfloat> linhas;

//Quantidade de coordenadas por vertice. Como estamos utilizando 2D, precisamos de duas coordenadas.
const GLuint coordenadasPorVertices=2;

//VARIAVEIS UTILIZADAS NO MAPEAMENTO DE DADOS DO PROGRAMA PARA OS SHADERS
GLuint program; //ID do programa, lembrando que um programa aqui eh a combinacao de vertex com fragment shader

int mainwindow=-1; //ID da janela de visualizacao criada pelo GLUT
GLuint width = 640, height = 480; //dimensoes da janela 

//Matriz que representa uma transformacao 2D
GLfloat transformacoes[] = {
	1.0f, 0.0f,
	0.0f, 1.0f
};

//Ponteiro para objeto que faz a comunicacao entre C++ e os shaders
ShaderProxy *proxy;

//Adiciona as coordenadas dos pontos de uma linha no plano representada por dois pontos
void adicionarPonto(GLfloat x1, GLfloat y1){
	linhas.push_back(x1);
	linhas.push_back(y1);
}

//Quantidade de vertices na lista linhas
GLfloat qtdVertices(){
	return linhas.size()/coordenadasPorVertices;
}

/**
*
* Reenvia informacoes dos vertices a serem processados nos shaders.
*/
void atualizarGeometria() {
	proxy->useProgram();
	proxy->setAttribute("posicao", &linhas[0], linhas.size() * sizeof(GLfloat), 
								coordenadasPorVertices);
	proxy->setUniform4f("cor", 1.0f, 0.0f, 0.0f, 0.0f);
	proxy->setUniformMatrix2fv("transformacoes", transformacoes, 1, GL_TRUE);
}

/**
*
* Configuracoes iniciais da aplicacao.
*
*/
int inicializar(void)
{
	try {		
		program = genProgram("shader.vs", "shader.fs");
		proxy = new ShaderProxy(program);
		atualizarGeometria();
	} catch(string error) {
		cout<<error<<endl;
		return 0;
	}

	return 1;
}

/**
* Implementacao do algoritmo de Cohen-Sutherland para codificacao das posicoes dos
* pontos em relacao a janela de selecao.
*/
GLuint codifica(GLfloat x, GLfloat y,
				GLfloat MINX, GLfloat MINY, 
				GLfloat MAXX, GLfloat MAXY)
{
	GLuint cod = 0;
	
	
	if (y > MAXY) //ponto acima da janela de selecao
		cod = 1; //codigo 0001
	else if (y < MINY) //ponto abaixo da janela de selecao
		cod = 2; //codigo 0010

	if (x > MAXX) //ponto a direita da janela de selecao
		cod = cod + 4; //codigo 0100
	else if (x < MINX)
			cod = cod + 8; //codigo 1000
	return cod;
}

//INICIO::FUNCOES DE MAPEAMENTO ENTRE COORDENADAS DO UNIVERSO E COORDENADAS DA TELA
GLuint toXScr(GLfloat x, GLfloat minXSRU, GLfloat maxXSRU){
	return  (GLuint)(x * width/(maxXSRU-minXSRU) + 0.5f);
}

GLuint toYScr(GLfloat y, GLfloat minYSRU, GLfloat maxYSRU) {
	return (GLuint)(y * (-height)/(maxYSRU-minYSRU) + 0.5f);
}

GLfloat toXSRU(GLuint x, GLfloat minXSRU, GLfloat maxXSRU) {
	GLfloat amp = maxXSRU - minXSRU;
	return  amp * x/width + minXSRU;
}

GLfloat toYSRU(GLuint y, GLfloat minYSRU, GLfloat maxYSRU) {
	GLfloat amp = maxYSRU - minYSRU;
	return  -(amp * y/height + minYSRU);
}

//FIM::FUNCOES DE MAPEAMENTO ENTRE COORDENADAS DO UNIVERSO E COORDENADAS DA TELA

/**
*
* Atualiza o desenho na tela de acordo com o modelo de cena atual.
*
*/
void atualizarTela()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	if (proxy) {
		proxy->useProgram();
		try {
			proxy->drawArrays(GL_LINES);
		} catch(string error) {
			cout<<error<<endl;
		}
	} 
	glutSwapBuffers();
}

/**
*
* Trata eventos do mouse.
*
*/
void tratarEventoDoMouse(int button, int state, int x, int y){
	cout<<"Mouse state is "<< ( state == GLUT_DOWN ?  "DOWN" : "UP" ) << endl; 
	cout<<"Mouse event on ("<<x<<","<<y<<") position of the screen!"<<endl;
	if (state == GLUT_DOWN){
		adicionarPonto(toXSRU(x, -1.0, 1.0), toYSRU(y, -1.0, 1.0));
		atualizarGeometria();
		glutPostRedisplay();
	}
}

/***
*
* Trata eventos do teclado.
*
*/
void tratarEventoDoTeclado(unsigned char key, int x, int y){
	cout<<"Key pressed: "<<key<<endl;
	cout<<"Point: ("<<x<<", "<<y<<")"<<endl;
}

/**
*
* Libera memoria usada pelo programa.
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
* Quando o usuário alterar o tamanho da janela, esta funcao serah
* chamada com os valores w e h correspondendo, respectivamente,
* aos novos valores da largura e da altura da janela.
*/
void redimensionarJanela(GLint w, GLint h){
	width = w;
	height = h;
	glViewport(0, 0, w, h); //Altera a viewport para oculpar toda a janela.
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
	mainwindow = glutCreateWindow("Desenhando Linhas");

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
		glutDisplayFunc(atualizarTela);
		glutMouseFunc(tratarEventoDoMouse);
		glutKeyboardFunc(tratarEventoDoTeclado);
		glutReshapeFunc(redimensionarJanela);
		glutMainLoop();
	}

	/* Se o ocasionalmente programa sair ,
	liberamos os recursos da memória e completaremos ele com sucesso*/
	return EXIT_SUCCESS;
}

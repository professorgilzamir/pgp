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
vector< vector<GLfloat> > objetos;

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

void adicionarPonto(vector<GLfloat> &pontos, GLfloat x, GLfloat y) {
	pontos.push_back(x);
	pontos.push_back(y);
}

void criar_elipse(vector<GLfloat> &pontos, GLfloat dx, GLfloat dy, GLfloat a, GLfloat b, GLuint N = 100)
{
	adicionarPonto(pontos, dx, dy);
	GLfloat x = dx+a;
	GLfloat y = dy;
	GLfloat passo = 2 * M_PI/N;
	for (GLfloat t = 0.0; t < 2 * M_PI; t += passo) {
		x = a * cos(t) + dx;
		y = b * sin(t) + dy;
		adicionarPonto(pontos, x, y);
	}
	adicionarPonto(pontos, dx+a, dy);
}

void criar_circulo(vector<GLfloat> &pontos, GLfloat x, 
					GLfloat y, GLfloat r, GLuint N=100) {
	criar_elipse(pontos, x, y, r, r, N);
}


void criar_retangulo(vector<GLfloat> &pontos, GLfloat x, 
							GLfloat y, GLfloat w, GLfloat h) {
	adicionarPonto(pontos, x, y);
	adicionarPonto(pontos, x+w, y);
	adicionarPonto(pontos, x+w, y+h);
	adicionarPonto(pontos, x, y+h);								
}


void carregar_objeto(vector<GLfloat> &pontos){
	try {
		proxy->useProgram();
		proxy->setAttribute("posicao", &pontos[0], pontos.size() * sizeof(GLfloat), 
								coordenadasPorVertices);
		proxy->setUniform4f("cor", 1.0f, 0.0f, 0.0f, 0.0f);
		proxy->setUniformMatrix2fv("transformacoes", transformacoes, 1, GL_TRUE);
	} catch(string error) {
		cout<<error<<endl;
	}
}

int inicializar(void)
{
	vector<GLfloat> elipse;
	vector<GLfloat> circulo;
	vector<GLfloat> retangulo;
	criar_elipse(elipse, 0.2f, 0.2f, 0.3f, 0.15f);
	criar_circulo(circulo, 0.0, -0.1, 0.1);
	criar_retangulo(retangulo, -0.5, -0.5, 0.1, 0.2);
	objetos.push_back(elipse);
	objetos.push_back(circulo);
	objetos.push_back(retangulo);

	try {		
		program = genProgram("shader.vs", "shader.fs");
		proxy = new ShaderProxy(program);
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
	if (proxy) {
		proxy->useProgram();
		try {
			for (int i = 0; i < objetos.size(); i++) {
				carregar_objeto(objetos[i]);
				proxy->drawArrays(GL_TRIANGLE_FAN);
			}
		} catch(string error) {
			cout<<error<<endl;
		}
	} 
	glutSwapBuffers();
}

void tratarEventoDoMouse(int button, int state, int x, int y){
		cout<<"Mouse state is "<< ( state == GLUT_DOWN ?  "DOWN" : "UP" ) << endl; 
		cout<<"Mouse event on ("<<x<<","<<y<<") position of the screen!"<<endl; 
}

void tratarEventoDoTeclado(unsigned char key, int x, int y){
	cout<<"Key pressed: "<<key<<endl;
	cout<<"Point: ("<<x<<", "<<y<<")"<<endl;
	transformacoes[0] += 0.1;
	//transformacoes[3] += 0.1;
	proxy->setUniformMatrix2fv("transformacoes", transformacoes);	
	glutPostRedisplay();
}

/**
*
* Programa memory desalocation.
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

void ajustarJanela(int w, int h) {
	width = w;
	height = h;
	//glViewport(0, 0, w, h);
	//glutPostRedisplay();
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
		glutReshapeFunc(ajustarJanela);
		glutMainLoop();
	}

	/* Se o ocasionalmente programa sair ,
	liberamos os recursos da memória e completaremos ele com sucesso*/
	return EXIT_SUCCESS;
}

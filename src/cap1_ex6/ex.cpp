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

const GLuint OBJ_RETANGULO = 0; //tipo de objeto geometrico
const GLuint OBJ_CIRCULO = 1; //tipo de objeto geometrico
const GLuint OBJ_ELIPSE = 2; //tipo de objeto geometrico


struct Objeto {
	vector<GLfloat> pontos;
	GLuint tipo;
	GLfloat *atributos;
	GLfloat largura;
	GLfloat altura;
	GLfloat x;
	GLfloat y;
	Objeto(const GLuint &tipo, const vector<GLfloat> &pontos, const GLfloat &x, 
			const GLfloat &y, const GLfloat &largura, const GLfloat &altura) {
		this->tipo = tipo;
		this->pontos = pontos;
		this->largura = largura;
		this->altura = altura;
		this->x = x;
		this->y = y;
	}
};

//Objeto selecionado pelo usuario
Objeto *selected = NULL;

//Onde serao armazenados os objetos (conjunto de pontos) a serem desenhados.
vector< Objeto* > objetos;

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

//INICIO DAS FUNCOES DE MAPEAMENTO ENTRE COORDENADAS DO UNIVERSO E COORDENADAS DA TELA
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
//FIM DAS FUNCOES DE MAPEAMENTO ENTRE COORDENADAS DO UNIVERSO E COORDENADAS DA TELA

//Adiciona um ponto plano
void adicionarPonto(vector<GLfloat> &dst, GLfloat x1, GLfloat y1){
	dst.push_back(x1);
	dst.push_back(y1);
}

vector<GLfloat> criar_elipse(GLfloat dx, GLfloat dy, GLfloat a, GLfloat b, GLint numberOfPoints=100){
	vector<GLfloat> dst;
	GLfloat x = 0.0f, y = 0.0f;
	adicionarPonto(dst, x+dx, y+dy);
	x = a * 1.0 + dx;
	GLfloat theta = 0.0f;
	GLfloat MAX = 2.0f * M_PI;
	GLfloat step = MAX/numberOfPoints;
	for (;theta <= MAX; theta += step) {
		x = a * cos(theta) + dx;
		y = b * sin(theta) + dy;
		adicionarPonto(dst, x, y);
	}
	adicionarPonto(dst, a * 1.0f + dx, dy);
	return dst;
}

vector<GLfloat> criar_circulo(GLfloat dx, GLfloat dy, GLfloat radio, GLint numberOfPoints=100) {
	return criar_elipse(dx, dy, radio, radio, numberOfPoints);
}

vector<GLfloat> criar_retangulo(GLfloat dx, GLfloat dy, GLfloat largura, GLfloat altura) {
	vector<GLfloat> pontos;
	GLfloat x = dx, y = dy;
	adicionarPonto(pontos, x, y);
	adicionarPonto(pontos, x+largura, y);
	adicionarPonto(pontos, x+largura, y+altura);
	adicionarPonto(pontos, x, y+altura);
	return pontos;
}

/**
*
* Configuracoes iniciais da aplicacao.
*
*/
int inicializar(void)
{
	vector<GLfloat> elipse = criar_elipse(0, 0, 0.5f, 0.2f);
	vector<GLfloat> circulo = criar_circulo(-0.5, -0.5, 0.1);
	vector<GLfloat> retangulo = criar_retangulo(0.7f, 0.7f, 0.2f, 0.2f);
	objetos.push_back(new Objeto(OBJ_ELIPSE, elipse, 0, 0, 0.5f, 0.2f));
	objetos.push_back(new Objeto(OBJ_CIRCULO, circulo, -0.5f, -0.5f, 0.1f, 0.1f));
	objetos.push_back(new Objeto(OBJ_RETANGULO, retangulo, 0.7f, 0.7f, 0.2f, 0.2f));
	try {
		program = genProgram("shader.vs", "shader.fs");
		proxy = new ShaderProxy(program);
	} catch(string error) {
		cout<<error<<endl;
		return 0;
	}
	return 1;
}

void carregarObjeto(vector<GLfloat> &objeto){
	proxy->useProgram();
	proxy->setAttribute("posicao", &objeto[0], objeto.size() * sizeof(GLfloat), 
								coordenadasPorVertices);
	proxy->setUniform4f("cor", 1.0f, 0.0f, 0.0f, 0.0f);
	proxy->setUniformMatrix2fv("transformacoes", transformacoes, 1, GL_TRUE);
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
		try {
			proxy->useProgram();
			for (int i = 0; i < objetos.size(); i++){
				vector<GLfloat> objeto = objetos[i]->pontos;
				carregarObjeto(objeto);
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
		GLfloat xw = toXSRU(x, -1.0f, 1.0f);
		GLfloat yw = toYSRU(y, -1.0f, 1.0f);
		
		if (state == GLUT_DOWN) {
			for (int i = 0; i < objetos.size(); i++) {
				Objeto *obj = objetos[i];
				if (obj->tipo == OBJ_CIRCULO){
					GLfloat cx = obj->x;
					GLfloat cy = obj->y;
					GLfloat r2 =  obj->largura * obj->largura;
					GLfloat d2 = (xw - cx) * (xw - cx) + (yw - cy) * (yw-cy);
					if (d2 <= r2){
						selected = obj;
					}
				} else if (obj->tipo == OBJ_ELIPSE) {

				} else if (obj->tipo == OBJ_RETANGULO) {

				}
			}
		} else if (state == GLUT_UP) {
			selected = NULL;
		}
}

void movimentoDoMouse(int x, int y) {
	if (selected != NULL) {
		GLfloat xw = toXSRU(x, -1.0f, 1.0f);
		GLfloat yw = toYSRU(y, -1.0f, 1.0f);
		
		if (selected->tipo == OBJ_CIRCULO){
			vector<GLfloat> circulo = criar_circulo(xw, yw, 0.1);
			selected->pontos = circulo;
			selected->x = xw;
			selected->y = yw;
		} else if (selected->tipo == OBJ_ELIPSE) {
			//TODO
		} else if (selected->tipo == OBJ_RETANGULO) {
			//TODO
		}
		glutPostRedisplay();
	}
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
	mainwindow = glutCreateWindow("Movimentando Objetos");

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
		glutMotionFunc(movimentoDoMouse);
		glutKeyboardFunc(tratarEventoDoTeclado);
		glutMainLoop();
	}

	/* Se o ocasionalmente programa sair ,
	liberamos os recursos da memória e completaremos ele com sucesso*/
	return EXIT_SUCCESS;
}

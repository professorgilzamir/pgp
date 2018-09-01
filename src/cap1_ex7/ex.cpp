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
const GLuint OBJ_GENERIC = 3;


struct Objeto {
	vector<GLfloat> pontos;
	GLuint tipo;
	GLfloat *atributos;
	GLfloat largura;
	GLfloat altura;
	GLfloat x;
	GLfloat y;
	GLenum primitive;
	GLfloat pointSize;

	Objeto(const GLuint &tipo, const vector<GLfloat> &pontos, const GLfloat &x, 
			const GLfloat &y, const GLfloat &largura, const GLfloat &altura) {
		this->tipo = tipo;
		this->pontos = pontos;
		this->largura = largura;
		this->altura = altura;
		this->x = x;
		this->y = y;
		this->pointSize = 1.0f;
		this->primitive = GL_TRIANGLE_FAN;
	}

	Objeto(const GLuint &tipo, const vector<GLfloat> &pontos, GLfloat pointSize = 1.0f, GLenum primitive=GL_LINES) {
		this->tipo = tipo;
		this->pontos = pontos;
		this->largura = 0.0f;
		this->altura = 0.0f;
		this->x = 0.0f;
		this->y = 0.0f;
		this->pointSize = pointSize;
		this->primitive = primitive;
	}	
};

//Objeto selecionado pelo usuario
Objeto *selected = NULL;

//Onde serao armazenados os objetos (conjunto de pontos) a serem desenhados.
vector< Objeto* > objetos;

//Quantidade de coordenadas por vertice. Como estamos utilizando 2D, precisamos de duas coordenadas.
const GLuint coordenadasPorVertices=3;

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


vector<GLfloat> genCilindro(GLfloat dx, GLfloat dy, GLfloat dz, GLfloat h, GLfloat r, GLuint M=100, GLuint N=100){
	GLfloat xy_step = 2.0f*M_PI/M;
	GLfloat z_step = 2.0f*h/N;
	vector<GLfloat> points;
	for (GLfloat t0 = -h; t0 <= h; t0 += z_step) {
		for (GLfloat t1 = 0.0f; t1 <= 2*M_PI; t1 += xy_step) {
			points.push_back(r*cos(t1)+dx);
			points.push_back(r*sin(t1)+dy);
			points.push_back(t0+dz);
		}
	}
	return points;
}


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

GLfloat* subarray(GLfloat a[], GLuint i, GLuint f) {
	GLfloat *out = new GLfloat[f-i+1];
	GLuint p = 0;
	for (GLuint k = i; k < f; k++) {
		out[p] = a[k];
		p += 1;
	}
	return out;
}

void linter(GLfloat t, GLfloat p1[2], GLfloat p2[2],  GLfloat result[]) {
	result[0] = (1-t) * p1[0] + t * p2[0];
	result[1] = (1-t) * p1[1] + t * p2[1];
}

void genBezierPoint(GLfloat t, GLfloat points[], GLfloat out[2], GLuint nc) {
	bool isFirst = true;		
	if (nc == 2) {
		GLfloat p1[2], p2[2];
		p1[0] = points[0];
		p1[1] = points[1];
		p2[0] = points[2];
		p2[1] = points[3];
		linter(t, p1, p2, out);
	} else {
		GLfloat *a = subarray(points, 0, 2*(nc-1));
		GLfloat *b = subarray(points, 2, 2*nc);
		GLfloat out1[2];
		GLfloat out2[2];
		genBezierPoint(t, a, out1, nc-1);
		genBezierPoint(t, b, out2, nc-1);
		out[0] = (1-t) * out1[0] + t * out2[0];
		out[1] = (1-t) * out1[1] + t * out2[1];
		delete []a;
		delete []b;
	}
}


vector<GLfloat> genBezierCurve(GLfloat control[], GLuint nc, GLuint R) {
	vector<GLfloat> out;
	GLfloat step = 1.0f/R;
	GLfloat p[2];
	for (GLfloat t = 0.0f; t < 1.0; t += step) {
		genBezierPoint(t, control, p, nc);
		out.push_back(p[0]);
		out.push_back(p[1]);
		cout<<"("<<p[0]<<","<<p[1]<<endl;
	}
	genBezierPoint(1.0, control, p, nc);
	out.push_back(p[0]);
	out.push_back(p[1]);
	
	return out;
}

//Propriedades da projecao perspectiva da camera
GLfloat fov = 45.0; //fov = field of view ou angulo de abertura da camera
GLfloat eye_position[3]; //Posicao da camera
GLfloat eye_orientation[2]; //Orientacao da camera
//MATRIZES QUE COMBINAM AS TRANSFORMACOES DE MODELO, CAMERA E PROJECAO : MODEL-VIEW-PROJECTION
GLfloat view[16]; //define a posicao e a orientacao da camera
GLfloat projection[16]; //define a projecao da camera


GLfloat matrix[16]; //contem o produto projection * view * model

/**
*
* Configuracoes iniciais da aplicacao.
*
*/
int inicializar(void)
{

	GLfloat aspect = (GLfloat)width/height;
	perspective(projection, fov, aspect, 0.01f, 10000.0f);
	identity(matrix);
	identity(view);

	//GLfloat control[] = {0.0f, 0.0f, 0.25f,  0.25f, 0.5f, 0.0f, 0.75, 0.25};
	//GLuint nc = 4;
	/*vector<GLfloat> elipse = criar_elipse(0, 0, 0.5f, 0.2f);
	vector<GLfloat> circulo = criar_circulo(-0.5, -0.5, 0.1);
	vector<GLfloat> retangulo = criar_retangulo(0.7f, 0.7f, 0.2f, 0.2f);*/
	//vector<GLfloat> bezier = genBezierCurve(control, nc, 100);
	//vector<GLfloat> controlpoints;
	//for (int i = 0; i < 2 * nc; i++) {
	//	controlpoints.push_back(control[i]);
	//}

	//objetos.push_back(new Objeto(OBJ_GENERIC, controlpoints, 10.f, GL_POINTS));
	//objetos.push_back(new Objeto(OBJ_GENERIC, bezier, 1.0f, GL_LINE_STRIP));
//	objetos.push_back(new Objeto(OBJ_ELIPSE, elipse, 0, 0, 0.5f, 0.2f));
//	objetos.push_back(new Objeto(OBJ_CIRCULO, circulo, -0.5f, -0.5f, 0.1f, 0.1f));
//	objetos.push_back(new Objeto(OBJ_RETANGULO, retangulo, 0.7f, 0.7f, 0.2f, 0.2f));
	
	std::vector<GLfloat> cilindro = genCilindro(0, 0, -0.4, 1000.0f, 0.3f, 2000, 2000);

	objetos.push_back(new Objeto(OBJ_GENERIC, cilindro, 2.0f, GL_POINTS));

	glEnable(GL_POINT_SMOOTH); //suavise as bordas do ponto
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE); //permite alterar o tamanho dos pontos no vertex shader


	try {
		program = genProgram("shader.vs", "shader.fs");
		proxy = new ShaderProxy(program);
	} catch(string error) {
		cout<<error<<endl;
		return 0;
	}
	return 1;
}

void carregarObjeto(vector<GLfloat> &objeto, GLfloat pointSize = 1.0f){
	proxy->useProgram();
	proxy->setAttribute("posicao", &objeto[0], objeto.size() * sizeof(GLfloat), 
								coordenadasPorVertices);
	proxy->setUniform4f("cor", 1.0f, 0.0f, 0.0f, 0.0f);
	proxy->setUniform1f("pointSize", pointSize);
	proxy->setUniformMatrix4fv("transformacoes", projection, 1, GL_TRUE);
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
				carregarObjeto(objeto, objetos[i]->pointSize);
				proxy->drawArrays(objetos[i]->primitive);
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

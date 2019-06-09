// Usando o padrão de saída fprintf */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>

/* Usando o GLUT com gerenciador de janelas */
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

/* COLOCAREMOS AS VARIAVEIS GLOBAIS AQUI MAIS TARDE */

struct Cor {
	float R;
	float G;
	float B;
	float A;
	
	Cor(const float &R, const float &G, const float &B, const float &A){
		this->R = R;
		this->G = G;
		this->B = B;
		this->A = A;
	}
	
	Cor(const Cor &cor){
		this->R = cor.R;
		this->G = cor.G;
		this->B = cor.B;
		this->A = cor.A;
	}
	
	string to_string(){
		stringstream ss;
		ss<<"("<<this->R<<", "<<this->G<<", "<<this->B<<", "<<this->A<< ")";
		return ss.str();
	}

	bool operator ==(const Cor &cor) {
		return this->R == cor.R && this->G == cor.G && this->B == cor.B && this->A == cor.A;
	}
	
	void operator=(const Cor &cor){
		this->R = cor.R;
		this->G = cor.G;
		this->B = cor.B;
		this->A = cor.A;
	}
};

const GLuint WIDTH = 640;
const GLuint HEIGHT = 480;

const GLfloat X_MIN = -400;
const GLfloat  X_MAX = 400;
const GLfloat  Y_MIN = -400;
const GLfloat  Y_MAX = 400;


float x = 0;
float y = 0;
float r = 100.0f;
float t = 0;
float inc = M_PI/1000.0;

GLubyte buffer[HEIGHT][WIDTH][4];

vector<float> px;
vector<float> py;

GLuint XToSCR(GLfloat x) {
	GLfloat dw = X_MAX - X_MIN;
	
	return (int) (WIDTH * (x - X_MIN)/dw + 0.5);
}

GLuint YToSCR(GLfloat y) {
	GLfloat dw = Y_MAX - Y_MIN;
	
	return (int) (HEIGHT * (y - Y_MIN)/dw + 0.5);
}



int inicializar(void)
{
  /* IMPLEMENTE AQUI */
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	return 1;
}


Cor getPixel(GLint x, GLint y){
	float R = buffer[y][x][0]/255.0;;
	float G = buffer[y][x][1]/255.0;
	float B = buffer[y][x][2]/255.0;
	float A = buffer[y][x][3]/255.0;
	Cor c(R, G, B, A);
	return c;
}

void setPixel(GLint x, GLint y, Cor c){
	buffer[y][x][0] = (GLuint) (c.R * 255);
	buffer[y][x][1] = (GLuint) (c.G * 255);
	buffer[y][x][2] = (GLuint) (c.B * 255);
	buffer[y][x][3] = (GLuint) (c.A * 255);
}

void read_buffer(){
	glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGBA,  GL_UNSIGNED_BYTE,  buffer);
}

void write_buffer(){
	glDrawPixels(WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE,  buffer);
}

void atualizarDesenho()
{
	/* IMPLEMENTE AQUI */
	glClear(GL_COLOR_BUFFER_BIT);
	read_buffer();
	
	for (int i = 0; i < px.size(); i ++) {
		float x = px[i];
		float y = py[i];
		setPixel((int)x, (int)y, Cor(1.0, 0.0, 0.0, 1.0));
	}
	
	write_buffer();
	glutSwapBuffers();
}

void finalizar()
{
  /* IMPLEMENTE AQUI */
}



void atualizar(GLint value){
	
	
	if (t <= 2 * M_PI){		
		for (int i = 0; i < 10; i++){
			px.push_back(XToSCR(r * cos(t) + x));
			py.push_back(YToSCR(r * sin(t)  + y));
			t += inc;
		}
	}

	
	if (value < 100000)
		glutTimerFunc(1, atualizar, value+1);
	
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
  /* Funções necessárias para iniciar a GLUT */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("Apenas uma Janela");
  glutTimerFunc(1, atualizar, 0);

  /* Quando as funções de inicialização são executadas sem erros,
  o programa pode iniciar os recursos */
  if (1 == inicializar())
  {
    /* Pode então mostrar se tudo correr bem */
    glutDisplayFunc(atualizarDesenho);
    glutMainLoop();
  }

  /* Se o ocasionalmente programa sair ,
  liberamos os recursos da memória e completaremos ele com sucesso*/
  finalizar();
  return EXIT_SUCCESS;
}

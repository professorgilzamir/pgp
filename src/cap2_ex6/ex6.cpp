/* Usando o padrão de saída fprintf */
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>

/* Usando o GLUT com gerenciador de janelas */
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

#include "shader.hpp"
#include "object.hpp"
#include <cmath>

using namespace std;

Shader *shader;
GraphicObject **circle;
Scene scene;
GLuint w = 400;
GLuint h = 400;


int mainwindow=-1;
GLsizei NCIRCLES = 1;
GLsizei CIRCLE_SIZE = 3;

string readfile(const char *name){
	ifstream source(name);
	string line;
	string content;
	if (source.is_open()){
		while (!source.eof()) {
			getline(source, line);
			content.append(line);
			content.append("\n");
		}
		source.close();
	}
	return content;
}

GraphicObject* newCircle(GLsizei N, GLfloat x, GLfloat y, GLfloat r) {
	GLfloat t = 0.0f;
	GLsizei n = N*2;
	GLsizei nc = N*4;
	GLfloat deltat = (2.0f * M_PI)/N; 
	GLfloat *vertices = new GLfloat[n];
	GLfloat *colors = new GLfloat[nc];
	GLsizei c = 0;
	GLsizei  v = 0;
	for (int i = 0; i < N; i++) {
		vertices[v] = x + r*cos(t);
		vertices[v+1] = y + r*sin(t);
		t = t + deltat;
		colors[c] = 1.0f; colors[c+1] = 0.0f; colors[c+2] = 0.0f; colors[c+3] = 1.0;
		c += 4;
		v += 2;
	}
	return new GraphicObject(shader, vertices, N, colors);
}


/* COLOCAREMOS AS VARIAVEIS GLOBAIS AQUI MAIS TARDE */
int inicializar(void)
{
	string vs = readfile("shader.vs");
	string fs = readfile("shader.fs");
	try{
		shader = new Shader(vs, fs);
		circle = new GraphicObject*[NCIRCLES];
		for (int i = 0; i < NCIRCLES; i++) {
			circle[i] = newCircle(CIRCLE_SIZE+i, 0.0f, 0.0f, 1.0);
			circle[i]->primitive = GL_LINE_LOOP;
			circle[i]->verticesAttribute = Attribute("coord2d");
			circle[i]->colorAttribute = Attribute("colors", 4);
			circle[i]->initShader();
			scene.objects.push_back( circle[i] );
		}
		return 1;
	} catch(string e) {
		cout<<e<<endl;
		return 0;
	}
}

void atualizarDesenho()
{
	if (shader != NULL) {
		scene.draw();
		glutSwapBuffers();
	}
}

void finalizar()
{
	if (circle != NULL) {
		for (int i = 0; i < NCIRCLES; i++) {
			circle[i]->deleteDynamic();
			delete circle[i];
		}
		delete [] circle;
		circle = NULL;
	}
		
	if (shader != NULL) {
		delete shader;
		shader = NULL;
	}
}

void fecharJanela() {
	finalizar();
	glutDestroyWindow(mainwindow);
	exit(0);
}

int main(int argc, char* argv[])
{


  if (argc > 1) {
  	NCIRCLES = stoi(argv[1]);
  }

  if (argc > 2) {
  	CIRCLE_SIZE = stoi(argv[2]);
  }

  /* Funções necessárias para iniciar a GLUT */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(w, h);
  mainwindow = glutCreateWindow("Meu Primeiro Triangulo");
  glViewport(0, 0, w, h);
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

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

using namespace std;

Shader *shader;
GraphicObject *triangle;
Scene scene;

GLfloat triangle_vertices[] = {
	0.0, 0.8,
	-0.8, -0.8,
	0.8, -0.8
};

GLfloat triangle_colors[] = {
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f
};

int mainwindow=-1;

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

/* COLOCAREMOS AS VARIAVEIS GLOBAIS AQUI MAIS TARDE */
int inicializar(void)
{
	string vs = readfile("shader.vs");
	string fs = readfile("shader.fs");
	try{
		shader = new Shader(vs, fs);
		triangle = new GraphicObject(shader,  triangle_vertices, 3, triangle_colors);
		triangle->verticesAttribute = Attribute("coord2d");
		triangle->colorAttribute = Attribute("colors", 4);
		triangle->initShader();
		scene.objects.push_back( triangle );
		return 1;
	} catch(string e) {
		cout<<e<<endl;
		return 0;
	}
}

void atualizarDesenho()
{
	scene.draw();
	glutSwapBuffers();
}

void finalizar()
{

	if (triangle != NULL) {
		delete triangle;
		triangle = NULL;	
	}

	if (shader != NULL) {
		delete shader;
		shader = NULL;
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
  mainwindow = glutCreateWindow("Meu Primeiro Triangulo");

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

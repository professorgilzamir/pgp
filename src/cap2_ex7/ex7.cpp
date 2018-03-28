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
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif

#include "shader.hpp"
#include "object.hpp"
#include <cmath>

using namespace std;

Shader *shader;
GraphicObject **cubo;
Scene scene;
GLuint w = 400;
GLuint h = 400;
GLsizei NCUBES = 1;

GLfloat world[] = {1.0f,   0.0f,   0.0f,  0.0f,
				   0.0f,   0.7f,  -0.7f,  0.0f,
				   0.0f,   0.7f,   0.7f,  0.0f,
				   0.0f,   0.0f,   0.0f,  1.0f};

GLfloat camera[] = {1.0f,  0.0f,  0.0f,  0.0f,
				    0.0f,  1.0f,  0.0f,  0.0f,
				    0.0f,  0.0f,  1.0f,  0.0f,
				    0.0f,  0.0f,  0.0f,  1.0f};


GLfloat projection[] = {1.0f,  0.0f,  0.0f,  0.0f,
				        0.0f,  1.0f,  0.0f,  0.0f,
				        0.0f,  0.0f,  0.0f,  0.0f,
				        0.0f,  0.0f,  1.0/0.4f,  1.0f};


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

GraphicObject* newCube(Shader *shader, GLfloat size) {
	GLsizei n = 8;
	GLsizei N = n * 3;
	GLsizei NC = n * 4; 
	GLfloat *vertices = new GLfloat[N];
	GLfloat *colors = new GLfloat[NC];
	GLfloat p = size/2.0;

	vertices[0] = p; vertices[1] = p; vertices[2] = p;
	colors[0] = 1.0f; colors[1] = 0.0f; colors[2] = 0.0f; colors[3] = 1.0;

	vertices[3] = -p; vertices[4] = p; vertices[5] = p;
	colors[4] = 1.0f; colors[5] = 0.0f; colors[6] = 0.0f; colors[7] = 1.0;
		
	vertices[6] = -p; vertices[7] = -p; vertices[8] = p;
	colors[8] = 1.0f; colors[9] = 0.0f; colors[10] = 0.0f; colors[11] = 1.0;
	
	vertices[9] = p; vertices[10] = -p; vertices[11] = p;
	colors[12] = 1.0f; colors[13] = 0.0f; colors[14] = 0.0f; colors[15] = 1.0;
	
	vertices[12] = p; vertices[13] = p; vertices[14] = -p;
	colors[16] = 1.0f; colors[17] = 0.0f; colors[18] = 0.0f; colors[19] = 1.0;
	
	vertices[15] = -p; vertices[16] = p; vertices[17] = -p;
	colors[20] = 1.0f; colors[21] = 0.0f; colors[22] = 0.0f; colors[23] = 1.0;
	
	vertices[18] = -p; vertices[19] = -p; vertices[20] = -p;
	colors[24] = 1.0f; colors[25] = 0.0f; colors[26] = 0.0f; colors[27] = 1.0;
	
	vertices[21] = p; vertices[22] = -p; vertices[23] = -p;
	colors[28] = 1.0f; colors[29] = 0.0f; colors[30] = 0.0f; colors[31] = 1.0;
	
	return new GraphicObject(shader, vertices, n, colors);
}


/* COLOCAREMOS AS VARIAVEIS GLOBAIS AQUI MAIS TARDE */
int inicializar(void)
{
	string vs = readfile("shader.vs");
	string fs = readfile("shader.fs");
	try{
		shader = new Shader(vs, fs);
		shader->setUniform(Uniform("projection"));
		shader->setUniform(Uniform("camera"));
		shader->setUniform(Uniform("world"));

		cubo = new GraphicObject*[NCUBES];
		for (int i = 0; i < NCUBES; i++) {
			cubo[i] = newCube(shader, 0.2f);
			cubo[i]->primitive = GL_POINTS;
			cubo[i]->verticesAttribute = Attribute("coord3d", 3);
			cubo[i]->colorAttribute = Attribute("colors", 4);
			cubo[i]->transformUniform = Uniform("model");
			cubo[i]->initShader();
			scene.objects.push_back( cubo[i] );
		}
		shader->init();
		shader->bindUDataMatrix4fv("projection",  1, true, projection);
		shader->bindUDataMatrix4fv("camera", 1, true, camera);
		shader->bindUDataMatrix4fv("world", 1, true, world);
		
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
	if (cubo != NULL) {
		for (int i = 0; i < NCUBES; i++) {
			cubo[i]->deleteDynamic();
			delete cubo[i];
		}
		delete [] cubo;
		cubo = NULL;
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

	/*
  if (argc > 1) {
  	NCIRCLES = stoi(argv[1]);
  }

  if (argc > 2) {
  	CIRCLE_SIZE = stoi(argv[2]);
  }*/

  /* Funções necessárias para iniciar a GLUT */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(w, h);
  mainwindow = glutCreateWindow("WireCube");
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

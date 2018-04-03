/* Usando o padrão de saída fprintf */
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
/* Usando o GLUT com gerenciador de janelas */

#include "matrixmath.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif

using namespace std;

GLfloat cube_vertices[] = {
	 1, -1,   1, //0
	 1,  1,   1, //1
	-1,  1,   1, //2
	-1, -1,   1, //3
	 1, -1,  -1, //4
	 1,  1,  -1, //5
	-1,  1,  -1, //6
	-1, -1,  -1, //7
};

GLuint cube_indices[] = {
		0, 1, 2, 3, //Face Fontal
		4, 5, 6, 7, //Face Traseira
		1, 6, 5, 2, //Face de Cima
		1, 3, 4, 7, //Face de Baixo
		2, 5, 4, 3, //Face da Esquerda
		0, 7, 6, 1, //Face da Direita 
};

GLfloat rotx45[] = {
	1, 0, 0, 0,
	0, 0.7f, -0.7f, 0.0f,
	0, 0.7f,  0.7f, 0.0f,
	0, 0.0f, 0.0f, 1.0f
};

GLfloat scale[] = {
	0.5f, 0, 0, 0,
	0, 0.5f, 0, 0,
	0, 0, 0.5f, 0,
	0, 0, 0, 1
};


GLfloat model[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};


GLuint program;
GLint attribute_coord3d;
GLint uniform_model;
int mainwindow=-1;
GLuint vabID=0;
GLuint eabID=0;

string* readfile(const char *name){
	ifstream source(name);
	string line;
	string *content = new string();
	if (source.is_open()){
		while (!source.eof()) {
			getline(source, line);
			content->append(line);
			content->append("\n");
		}
		source.close();
	}
	return content;
}

/* COLOCAREMOS AS VARIAVEIS GLOBAIS AQUI MAIS TARDE */

int inicializar(void)
{
	GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	string *shadervs = readfile("shader.vs");
	const char* vscontent = shadervs->data();
	glShaderSource(vs, 1, &vscontent, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);

	if (0 == compile_ok){
		delete shadervs;
		fprintf(stderr, "Error in vertex shader\n");
		return 0;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	string *shaderfs = readfile("shader.fs");
	const char *fscontent = shaderfs->data();
	glShaderSource(fs, 1, &fscontent, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
	if (0==compile_ok) {
		delete shadervs;
		delete shaderfs;
		fprintf(stderr, "Error in fragment shader\n");
		return 0;
	}

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);



	delete shadervs;
	delete shaderfs;

	if (!link_ok) {
		fprintf(stderr, "Error in glLinkProgram");
		delete shadervs;
		delete shaderfs;		
		return 0;
	}

	glGenBuffers(1, &vabID);
	glBindBuffer(GL_ARRAY_BUFFER, vabID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &eabID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eabID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	const char *attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord3d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}


	const char *uniform_model_name = "model";
	uniform_model = glGetUniformLocation(program, uniform_model_name);

	multMatrix4(scale, rotx45, model);

	return 1;
}

void atualizarDesenho()
{

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);
	

	glUniformMatrix4fv(uniform_model, 1, GL_FALSE, model);

	glBindBuffer(GL_ARRAY_BUFFER, vabID);
	glEnableVertexAttribArray(attribute_coord3d);
	glVertexAttribPointer(attribute_coord3d,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				NULL
				);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eabID);

	
	//glDrawArrays(GL_LINE_LOOP, 0, 3);

	 glDrawElements(
     	GL_QUADS,      // mode
     	24,    // count
     	GL_UNSIGNED_INT,   // type
    	 (void*)0           // element array buffer offset
	 );

	glDisableVertexAttribArray(attribute_coord3d);
	glutSwapBuffers();
}

void finalizar()
{
  glDeleteProgram(program);
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
  mainwindow = glutCreateWindow("Meu Primeiro Cubo Minha Vida");

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

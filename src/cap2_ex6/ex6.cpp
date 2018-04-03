/* Usando o padrão de saída fprintf */
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
/* Usando o GLUT com gerenciador de janelas */

#include "matrixmath.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif

using namespace std;

GLfloat fov = 45.0 * M_PI/180.0f;
GLfloat rot = 0.0f;
GLfloat rotInc = 10 * M_PI/180.0f;
GLfloat k = 1.0f;
GLfloat cube_vertices[] = {
	 1 * k, -1 * k,   1 * k, //0
	 1 * k,  1 * k,   1 * k, //1
	-1 * k,  1 * k,   1 * k, //2
	-1 * k, -1 * k,   1 * k, //3
	-1 * k, -1 * k,  -1 * k, //4
	-1 * k,  1 * k,  -1 * k, //5
	 1 * k,  1 * k,  -1 * k, //6
	 1 * k, -1 * k,  -1 * k, //7
};

GLfloat cube_colors[] = {
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1
};

GLuint cube_indices[] = {
		0, 1, 2, 3, //Face Fontal
		4, 5, 6, 7, //Face Traseira
		1, 6, 5, 2, //Face de Cima
		0, 3, 4, 7, //Face de Baixo
		2, 5, 4, 3, //Face da Esquerda
		7, 6, 1, 0, //Face da Direita 
};



GLfloat scale[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

GLfloat translation[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, -10.0f,
	0, 0, 0, 1
};

GLfloat far = 100.0f;
GLfloat near = 0.001;


GLfloat proj[] = {
	1/tan(fov/2.0), 0, 0, 0,
	0, 1/tan(fov/2.0), 0, 0,
	0, 0, (far+near)/(near-far), (2 * far * near)/(near-far),
	0, 0, -1, 0
};

GLfloat matrix[] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};


GLuint program;
GLint attribute_coord3d;
GLint attribute_color3d;
GLint uniform_matrix;

int mainwindow=-1;
GLuint vabID=0;
GLuint eabID=0;
GLuint cabID=0;

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

//		GLint maxLength = 0;
//		std::vector<GLchar> errorLog(maxLength);

//		glGetShaderInfoLog(vs, maxLength, &maxLength, &errorLog[0]);
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


	glGenBuffers(1, &cabID);
	glBindBuffer(GL_ARRAY_BUFFER, cabID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &eabID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eabID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	const char *attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord3d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}


	const char *attribute_color_name = "color3d";
	attribute_color3d = glGetAttribLocation(program, attribute_color_name);
	if (attribute_color3d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_color_name);
		return 0;
	}


	const char *uniform_matrix_name = "matrix";
	uniform_matrix = glGetUniformLocation(program, uniform_matrix_name);

	GLfloat rotx45[] = {
		1, 0, 0, 0,
		0, cos(rot), -sin(rot), 0.0f,
		0, sin(rot),  cos(rot), 0.0f,
		0, 0.0f, 0.0f, 1.0f
	};

	identity(matrix);
	multMatrix4(matrix, proj, matrix);
	multMatrix4(matrix, translation, matrix);
	multMatrix4(matrix, scale, matrix);
	multMatrix4(matrix, rotx45, matrix);


	return 1;
}

void atualizarDesenho()
{

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	
	glUniformMatrix4fv(uniform_matrix, 1, GL_TRUE, matrix);

	glBindBuffer(GL_ARRAY_BUFFER, cabID);
	glEnableVertexAttribArray(attribute_color3d);
	glVertexAttribPointer(attribute_color3d,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				NULL
				);


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

	glDisableVertexAttribArray(attribute_color3d);
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

void rotate(unsigned char key, int x, int y) {
	if (key == 'r') {
		rot = rot + rotInc;

		GLfloat rotx45[] = {
			1, 0, 0, 0,
			0, cos(rot), -sin(rot), 0.0f,
			0, sin(rot),  cos(rot), 0.0f,
			0, 0.0f, 0.0f, 1.0f
		};

		identity(matrix);
		multMatrix4(matrix, proj, matrix);
		multMatrix4(matrix, translation, matrix);
		multMatrix4(matrix, scale, matrix);
		multMatrix4(matrix, rotx45, matrix);

		atualizarDesenho();
	}
}

int main(int argc, char* argv[])
{
	/* Funções necessárias para iniciar a GLUT */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	mainwindow = glutCreateWindow("Meu Primeiro Cubo Minha Vida");

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
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
		glutKeyboardFunc(rotate);
		glutMainLoop();
	}

	/* Se o ocasionalmente programa sair ,
	liberamos os recursos da memória e completaremos ele com sucesso*/
	return EXIT_SUCCESS;
}

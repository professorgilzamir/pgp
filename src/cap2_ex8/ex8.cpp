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
using namespace matrixmath;

GLfloat fov = 45.0;

GLfloat eye_position[3];
GLfloat eye_orientation[2];


GLfloat cube_vertices[] = {
	1.000000f, -1.000000f, -1.000000f,
	1.000000f, -1.000000f, 1.000000f,
	-1.000000f, -1.000000f, 1.000000f,
	-1.000000f, -1.000000f, -1.000000f,
	1.000000f, 1.000000f, -1.000000f,
	0.999999f, 1.000000f, 1.000001f,
	-1.000000f, 1.000000f, 1.000000f,
	-1.000000f, 1.000000f, -1.000000f,
	1.224745f, -1.224745f, -0.000000f,
	0.000000f, -1.224745f, -1.224745f,
	1.224745f, 0.000000f, -1.224744f,
	-0.000000f, -1.224745f, 1.224745f,
	1.224744f, -0.000000f, 1.224745f,
	-1.224745f, -1.224745f, -0.000000f,
	-1.224745f, -0.000000f, 1.224745f,
	-1.224745f, 0.000000f, -1.224745f,
	1.224745f, 1.224745f, 0.000001f,
	0.000000f, 1.224745f, -1.224745f,
	-0.000001f, 1.224745f, 1.224745f,
	-1.224745f, 1.224745f, -0.000000f,
	0.000000f, 0.000000f, -1.732050f,
	-1.732051f, 0.000000f, -0.000000f,
	-0.000000f, -0.000000f, 1.732051f,
	1.732051f, 0.000000f, 0.000000f,
	-0.000000f, 1.732051f, 0.000000f,
	0.000000f, -1.732051f, -0.000000f
};


GLfloat cube_colors[] = {
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	1, 0, 0,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1,
	0, 0, 1
};

GLuint cube_indices[] = {
	26, 12, 3, 14,
	25, 20, 7, 19,
	24, 17, 6, 13,
	23, 19, 7, 15,
	22, 20, 8, 16,
	21, 10, 4, 16,
	18, 21, 16, 8,
	5, 11, 21, 18,
	11, 1, 10, 21,
	14, 22, 16, 4,
	3, 15, 22, 14,
	15, 7, 20, 22,
	12, 23, 15, 3,
	2, 13, 23, 12,
	13, 6, 19, 23,
	9, 24, 13, 2,
	1, 11, 24, 9,
	11, 5, 17, 24,
	17, 25, 19, 6,
	5, 18, 25, 17,
	18, 8, 20, 25,
	10, 26, 14, 4,
	1, 9, 26, 10,
	9, 2, 12, 26
};



GLfloat scale[16];
GLfloat translation[16];

GLfloat model[16];
GLfloat view[16];
GLfloat projection[16];


GLfloat matrix[16];

GLuint program;
GLint attribute_coord3d;
GLint attribute_color3d;
GLint uniform_matrix;

int mainwindow=-1;
GLuint vabID=0;
GLuint eabID=0;
GLuint cabID=0;
GLuint width = 640, height = 480;


void initializeMatrix() {
	//ortho(projection, -100, 100, -100, 100, 0.0001, 1000.0);

	for (int i = 0; i < 96; i++) {
		cube_indices[i]--;
	}


	identity(model);

	GLfloat aspect = (GLfloat)width/height;
	perspective(projection, fov, aspect, 0.01f, 10000.0f);

	scaleMatrix4(scale,  20, 20, 20);
	translationMatrix4(translation, 0, 0, -200);
	multMatrix4(model, translation, model);
	multMatrix4(model, scale, model);
}

void updateMatrix() {
	identity(matrix);
	identity(view);


	GLfloat rotViewX[16];
	GLfloat rotViewY[16];

	rotationXMatrix4(rotViewX, eye_orientation[0]);
	rotationYMatrix4(rotViewY, eye_orientation[1]);

	//EXERCICIO IMPLEMENTAR MOVIMENTO DA CÂMERA (2 PONTOS NA SEGUNDA AP)

	multMatrix4(view, rotViewY, view);
	multMatrix4(view, rotViewX, view);

	transposeMatrix4(view, view);

	multMatrix4(matrix, projection, matrix);
	multMatrix4(matrix, view, matrix);
	multMatrix4(matrix, model, matrix);
}

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
	initializeMatrix();
	updateMatrix();
	return 1;
}

void atualizarDesenho()
{

	glClearColor(1.0, 1.0, 1.0, 1.0);
	//glClearDepth(1.0f);
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
     	96,    // count
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


void eventoDeTeclado(unsigned char key, int x, int y) {
	if (key == 'i') {
		eye_orientation[0] += 10.0f;
	} else if (key == 'k') {
		eye_orientation[0] -= 10.0f;
	}

	if (key == 'j') {
		eye_orientation[1] += 10;
	} else if (key == 'l') {
		eye_orientation[1] -= 10;
	}


	updateMatrix();
	atualizarDesenho();
}

int main(int argc, char* argv[])
{
	/* Funções necessárias para iniciar a GLUT */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(width, height);
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
		glutKeyboardFunc(eventoDeTeclado);
		glutMainLoop();
	}

	/* Se o ocasionalmente programa sair ,
	liberamos os recursos da memória e completaremos ele com sucesso*/
	return EXIT_SUCCESS;
}

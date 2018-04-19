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
#include "utils.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif

using namespace std;
using namespace matrixmath;
using namespace utils;

#include "cubo.cpp"

//PROPRIEDADES DA CAMERA

//Propriedades da projecao perspectiva da camera
GLfloat fov = 45.0; //fov = field of view ou angulo de abertura da camera
GLfloat eye_position[3]; //Posicao da camera
GLfloat eye_orientation[2]; //Orientacao da camera


//PROPRIEDADES DOS OBJETOS (MODEL) NA CENA
GLfloat scale[16]; //dimensao do objeto
GLfloat translation[16]; //deslocamento em relacao a origem da cena


//MATRIZES QUE COMBINAM AS TRANSFORMACOES DE MODELO, CAMERA E PROJECAO : MODEL-VIEW-PROJECTION
GLfloat model[16]; //define as caracteristicas geometricas do modelo
GLfloat view[16]; //define a posicao e a orientacao da camera
GLfloat projection[16]; //define a projecao da camera


GLfloat matrix[16]; //contem o produto projection * view * model


//VARIAVEIS UTILIZADAS NO MAPEAMENTO DE DADOS DO PROGRAMA PARA OS SHADERS

GLuint program; //ID do programa, lembrando que um programa aqui eh a combinacao de vertex com fragment shader
GLint attribute_coord3d; //id do atributo coord3d (vertex shader)
GLint attribute_normal3d;
GLint uniform_materialColor; //id uniform uColor (vertex shader)
GLint uniform_model; //id do uniform model (vertex shader)
GLint uniform_view; //id do uniform view (vertex shader)
GLint uniform_projection; //id do uniform projection (vertex shader)


GLfloat normais[78];

int mainwindow=-1; //ID da janela de visualizacao criada pelo GLUT
GLuint vabID=0; //ID do VAB (Vertex Array Buffer)
GLuint nabID=0; //ID do buffer de normais (Normal Array Buffer)
GLuint eabID=0; //ID do EAB (Element Array Buffer)
GLuint width = 640, height = 480;

//FUNCOES DO PROGRAMA

/**
* 
* Configuracao inicial das matrizes MODEL-VIEW-PROJECTION
*
*/
void initializeMatrix() {
	//ortho(projection, -100, 100, -100, 100, 0.0001, 1000.0);
	identity(model);
	GLfloat aspect = (GLfloat)width/height;
	perspective(projection, fov, aspect, 0.01f, 10000.0f);

	scaleMatrix4(scale,  20, 20, 20);
	translationMatrix4(translation, 0, 0, -200);
	multMatrix4(model, translation, model);
	multMatrix4(model, scale, model);
}


/**
*
* Atualizacao das matrizes de acordo com as acoes do usuario
*
*/
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
}


/**
*
* Leitura de um arquivo com path name e armazenamento do conteudo em um array de strings
*
*/
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

/**
*
* Compila os shaders, monta o programa e cria buffers iniciais
*
*/
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubo_vertices), cubo_vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	getNormals(cubo_numberOfVertices, cubo_faceindex, cubo_normalindex, cubo_normals, cubo_numberOfFaces, 
							cubo_verticesInFace, normais);
	
	for (int i = 0; i < 26; i++) {
		int idx = i * 3;
		cout<<normais[idx]<<", "<<normais[idx+1]<<", "<<normais[idx+2]<<endl;
	}


	glGenBuffers(1, &nabID);
	glBindBuffer(GL_ARRAY_BUFFER, nabID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normais), normais, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &eabID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eabID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubo_faceindex), cubo_faceindex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	const char *attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord3d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}


	const char *attribute_normal_name = "normal3d";
	attribute_normal3d = glGetAttribLocation(program, attribute_normal_name);
	if (attribute_normal3d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_normal_name);
		return 0;
	}

	const char *uniform_materialColor_name = "materialColor";
	uniform_materialColor = glGetUniformLocation(program, uniform_materialColor_name);

	const char *uniform_model_name = "model";
	uniform_model = glGetUniformLocation(program, uniform_model_name);

	const char *uniform_view_name = "view";
	uniform_view = glGetUniformLocation(program, uniform_view_name);

	const char *uniform_projection_name = "projection";
	uniform_projection = glGetUniformLocation(program, uniform_projection_name);

	initializeMatrix();
	updateMatrix();
	return 1;
}

/**
*
* Atualiza o desenho na tela de acordo com o modelo de cena atual.
*
*/
void atualizarDesenho()
{

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);

	glUniformMatrix4fv(uniform_model, 1, GL_TRUE, model);
	glUniformMatrix4fv(uniform_view, 1, GL_TRUE, view);
	glUniformMatrix4fv(uniform_projection, 1, GL_TRUE, projection);

	glUniform4f(uniform_materialColor, 1.0f, 0.0f, 0.0f, 1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, nabID);
	glEnableVertexAttribArray(attribute_normal3d);
	glVertexAttribPointer(attribute_normal3d,
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


	 glDrawElements(
     	GL_QUADS,      // mode
     	96,    // count
     	GL_UNSIGNED_INT,   // type
    	 (void*)0           // element array buffer offset
	 );

	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_normal3d);
	glutSwapBuffers();
}

/**
*
* Desaloca o programa gerado.
*
*/
void finalizar()
{
  glDeleteProgram(program);
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
* Captura eventos do teclado que representam acoes do usuario.
*
*/
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

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
#include <GL/glut.h>
#endif

using namespace std;

GLfloat triangle_vertices[] = {
	0.0, 0.8,
	-0.8, -0.8,
	0.8, -0.8
};

GLuint program;
GLint attribute_coord2d;


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


	const char *attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord2d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}

	return 1;
}

void atualizarDesenho()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord2d);
	glVertexAttribPointer(attribute_coord2d,
				2,
				GL_FLOAT,
				GL_FALSE,
				0,
				triangle_vertices
				);

	glDrawArrays(GL_LINE_LOOP, 0, 3);
	glDisableVertexAttribArray(attribute_coord2d);
	glutSwapBuffers();
}

void finalizar()
{
  glDeleteProgram(program);
}

int main(int argc, char* argv[])
{
  /* Funções necessárias para iniciar a GLUT */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(640, 480);
  glutCreateWindow("Meu Primeiro Triangulo");

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
    glutDisplayFunc(atualizarDesenho);
    glutMainLoop();
  }

  /* Se o ocasionalmente programa sair ,
  liberamos os recursos da memória e completaremos ele com sucesso*/
  finalizar();
  return EXIT_SUCCESS;
}

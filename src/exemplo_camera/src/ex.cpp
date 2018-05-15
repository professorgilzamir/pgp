#include <shaderutils.hpp>
#include <scene.hpp>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif

using namespace shaderutils;
using namespace scene;

ShaderProxy *proxy;
ShadedObject *triangle;
Camera *camera;

GLfloat pontos[] = {
	0.0f, 0.0f,
	50.0f, 0.0f,
	50.0f, 50.0f
};

void init() {
	 proxy = new ShaderProxy(genProgram("shader.vs", "shader.fs"));
	 proxy->useProgram();
	 proxy->setAttribute("posicao", pontos, sizeof(pontos), 2);
	 triangle = new ShadedObject("triangle", proxy);
	 triangle->transformName = "transform";
	 triangle->update();
	 camera = new Camera(proxy);
	 camera->setOrtho(-100.0f, 100.0f, -100.0f, 100.0f, 0.001, 1000);
	 camera->update();
}

void onDraw() {
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	proxy->useProgram();
	camera->update();
	proxy->drawArrays(GL_TRIANGLES);
	glutSwapBuffers();
}

void onKeyPress(unsigned char key, int x, int y){
	triangle->data.rotateZ(1);
	triangle->update();
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Hello ShaderProxy");
	#ifndef __APPLE__
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK)
	{
		fprintf(stderr, "Erro: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}
	#endif
	init();

	glutDisplayFunc(onDraw);
	glutKeyboardFunc(onKeyPress);
	glutMainLoop();

	delete camera;
	camera = 0;

	delete triangle;
	triangle = 0;

	proxy->close();
	delete proxy;
	proxy = 0;
	return 1;
}

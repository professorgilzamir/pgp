#include "shaderutils.hpp"
#include <GLUT/glut.h>

using namespace shaderutils;

ShaderProxy *proxy;

GLfloat pontos[] = {
	0.0f, 0.0f,
	0.5f, 0.0f,
	0.0f, 0.5f
};

void init() {
	 proxy = new ShaderProxy(genProgram("shader.vs", "shader.fs"));
	 proxy->setAttribute("posicao", pontos, sizeof(pontos), 2);
}

void onDraw() {
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	proxy->useProgram();
	proxy->drawArrays(GL_TRIANGLES);
	glutSwapBuffers();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Hello ShaderProxy");

	init();

	glutDisplayFunc(onDraw);
	glutMainLoop();

	proxy->close();
	delete proxy;
	proxy = 0;
	return 1;
}

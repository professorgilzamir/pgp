#include "shaderutils.hpp"
#include "scene.hpp"
#include <GLUT/glut.h>

using namespace shaderutils;
using namespace scene;

ShaderProxy *proxy;
ShadedObject *triangle;


GLfloat pontos[] = {
	0.0f, 0.0f,
	0.5f, 0.0f,
	0.5f, 0.5f
};

void init() {
	 proxy = new ShaderProxy(genProgram("shader.vs", "shader.fs"));
	 proxy->useProgram();
	 proxy->setAttribute("posicao", pontos, sizeof(pontos), 2);
	 triangle = new ShadedObject("triangle", proxy);
	 triangle->transformName = "transform";
	 triangle->update();
}

void onDraw() {
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	proxy->useProgram();
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

	init();

	glutDisplayFunc(onDraw);
	glutKeyboardFunc(onKeyPress);
	glutMainLoop();

	delete triangle;
	triangle = 0;

	proxy->close();
	delete proxy;
	proxy = 0;
	return 1;
}

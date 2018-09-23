/***************************************************************************************************************
Copyright (C) Gilzamir Gomes (gilzamir@gmail.com).

Introducao.
-----------
Este programa eh um objeto educacional para a disciplina de computacao grafica. O objetivo deste objeto 
eh compreender e implementar as superficies de bezier ou programas que utilizam esta superficie. Procura-se
com isso fazer o aluno desenvolver capacidade de programacao grafica em um nivel algoritmo, compreendendo os aspectos
subjacentes dos programas que utiliza. Como objeto educacional, a utilidade deste programa nao esta no programa 
executavel em si, mas sim em seu codigo-fonte. O estudante deve ler os comentarios, tecer hipoteses sobre o 
funcionamento do programa e alterar, compilar e executar o programa para verificar as hipoteses sob analise.

Advertencia.
------------
Este programa foi desenvolvido exclusivamente para fins educacionais. Nao dou qualquer garantia de que este
programa servira para qualquer outro proposito e, dessa forma, nao me responsabilizo por eventual mal uso que dele
seja feito. Por exemplo, nao eh de responsabilidade do autor deste programa eventuais perdas de arquivo do usuario
causadas por alteracao indevida do codigo subjacente. Antes de executa-lo, leia todos comentarios e tenha certeza
de nao ter feito nenhuma alteracao prejudicial ao seu computador.

Instrucoes.
-----------
Leiam todos os comentarios. Experimentem o programa antes de tentar modifica-lo. Ao executar o programa
pela primeira vez (sem altera-lo), sera exibida uma superficie de bezier que o usuario pode movimenta-la
na tela. Para movimentar o objeto na tela, siga as instrucoes:

x: rotaciona o objeto em torno do eixo x no sentido anti-horario.
y: rotaciona o objeto em torno do eixo y no sentido anti-horario.
z: rotaciona o objeto em torno do eixo z no sentido anti-horario.
X: rotaciona o objeto em torno do eixo x no sentido horario.
Y: rotaciona o objeto em torno do eixo y no sentido horario.
Z: rotaciona o objeto em torno do eixo z no sentido horario.
a: desloca o objeto para a esquerda.
d: desloca o objeto para a direita.
w: empura o objeto para frente.
s: empura o objeto para tras.

Leia todos os comentarios neste programa, faca alteracoes de acordo com o que pedem os exercicios.
Recompile o codigo para cada alteracao e observe se o resultado pretendido foi alcancado. Para compilar
o programa, supodno que todos os pre-requisitos estao satisfeitos, faca no Linux:

make

no MacOSX faca:

make PLAT=macosx

IMPORTANTE:

Se nada acontecer com a execucao do comando make, limpe a compilacao atual com o comando make clean. Em qualquer
dos dois sistema operacionais suportados, faca:


make clean


****************************************************************************************************************


/* Usando o padrão de saída fprintf */
#include <cstdio> //entrada e saida a moda antiga, do C.
#include <cstdlib> //comandos do sistema a moda antiga, do C. Por exemplo, exit(0).
#include <cmath> //comandos matematicos a moda antiga, do C. Por exemplo: sin(3.14/2.0) - calcula seno em radianos.
#include <iostream> //entrada e saida a moda C++.
#include <vector> //lista estatica de objetos, equivalente aas array lists da Ling. Java.

/* Usando o GLUT com gerenciador de janelas */
#include "matrixmath.hpp" //necessario para operacoes matriciais
#include "shaderutils.hpp" //necessario para manipulacao de shaders

#ifdef __APPLE__ //necessario para funcionar no macosx
#include <GLUT/glut.h> //o modo de usar a biblioteca GLUT no macosx.
#else
#include "GL/glew.h" //o modo de usar as extensoes programaveis da OpenGL 2.1 no Linux.
#include <GL/freeglut.h> //o modo de usar a biblioteca glut no Linux.
#endif

using namespace std; //espaco de nome da biblioteca padrao do c++
using namespace matrixmath; //espaco de nome da biblioteca matrixmath
using namespace shaderutils; //espaco de nome da biblioteca shaderutils

const GLuint OBJ_RETANGULO = 0; //tipo de objeto geometrico: retangulo 2d.
const GLuint OBJ_CIRCULO = 1; //tipo de objeto geometrico: circulo 2d.
const GLuint OBJ_ELIPSE = 2; //tipo de objeto geometrico: elipse 2d.
const GLuint OBJ_GENERIC = 3; //tipo de objeto generico: o usuario define as propriedades do objeto.


/**
* Uma struct que guarda informacoes do objeto a ser renderizado.
*/
struct Objeto {
	vector<GLfloat> pontos; //lista de pontos do objeto.
	/*
		Tipo de objeto (se eh um objeto plano pre-definido (OBJ_RETANGULO, OBJ_CIRCULO, OBJ_ELIPSE)
		ou se eh um objeto generico (OBJ_GENERIC) em que as primitivas sao definidas pelo usuario.
	*/
	GLuint tipo; 

	GLfloat *atributos; //nao utilizado
	
	/*
		Em caso de objetos planos pre-definidos, representa a altura do objeto.
		Nao eh usado com OBJ_GENERIC. Em caso de circulos, representa o raio do circulo
		e neste caso altura eh igual a largura.
	*/
	GLfloat altura; 
	
	/*
		Em caso de objetos planos pre-definidos, representa a largura do objeto.
		Nao eh usado com OBJ_GENERIC. Em caso de circulos, representa o raio do circulo
		e neste caso altura eh igual a largura.
	*/
	GLfloat largura;
	
	/*
		No caso de objetos planos pre-definidos, os atributos x e y representam a localizacao do objeto.
		Se for um retangulo, pode ser o seu canto superior esquerdo. No caso de circulo,
		eh o centro do circulo.
	*/
	GLfloat x;
	GLfloat y;
	

	/**
		Indica como a OpenGL deve considerar a topologia  do objeto.
		Por exemplo, pode-se usar GL_POINTS, GL_LINES, GL_TRIANGLES, dentre outras primitivas.
	*/
	GLenum primitive;  
	

	GLfloat pointSize; //define quantos pixels devem ser usados para renderizacao de cada ponto.
	
	GLuint nb_coordinates; //quantidade de coordenadas por vertice, em objetos planos esse valor deve ser 2. Em objetos espaciais, deve ser 3.

	/**
		Para construir um objeto plano especifico, use este construtor.
	*/
	Objeto(const GLuint &tipo, const vector<GLfloat> &pontos, const GLfloat &x, 
			const GLfloat &y, const GLfloat &largura, const GLfloat &altura) {
		this->tipo = tipo;
		this->pontos = pontos;
		this->largura = largura;
		this->altura = altura;
		this->x = x;
		this->y = y;
		this->pointSize = 1.0f;
		this->nb_coordinates = 3;
		this->primitive = GL_TRIANGLE_FAN;
	}

	/**
		Para construir um objeto generico, use este construtor.
	*/
	Objeto(const GLuint &tipo, const vector<GLfloat> &pontos, GLfloat pointSize = 1.0f, GLenum primitive=GL_LINES) {
		this->tipo = tipo;
		this->pontos = pontos;
		this->largura = 0.0f;
		this->altura = 0.0f;
		this->x = 0.0f;
		this->y = 0.0f;
		this->pointSize = pointSize;
		this->primitive = primitive;
		this->nb_coordinates = 3;
	}	
};

//Objeto selecionado pelo usuario
Objeto *selected = NULL;

//Onde serao armazenados os objetos (conjunto de pontos) a serem desenhados.
vector< Objeto* > objetos;

//VARIAVEIS UTILIZADAS NO MAPEAMENTO DE DADOS DO PROGRAMA PARA OS SHADERS
GLuint program; //ID do programa, lembrando que um programa aqui eh a combinacao de vertex com fragment shader

int mainwindow=-1; //ID da janela de visualizacao criada pelo GLUT
GLuint width = 640, height = 480; //dimensoes da janela 

/**
	viewrotx = rotacao do objeto em torno do eixo x.
	viewroty = rotacao do objeto em torno do eixo y.
	viewrotz = rotacao do objeto em torno do eixo z.
	dx = posicao inicial do objeto em relacao ao eixo x.
	dy = posicao inicial do objeto em relacao ao eixo y.
	dz = posical inicial do objeto em relacao ao eixo z.

*/
GLfloat  viewrotx=25.0f, viewroty=0.0f, viewrotz=0.0f, dx=0.0, dy=0.0f, dz=-0.2;

//Matriz que representa uma transformacao 2D
GLfloat transformacoes[] = {
	1.0f, 0.0f,
	0.0f, 1.0f
};

/*
	Coeficientes do polinomio de Bernstein de acordo com o triangulo de pascal.
	Observe que o tringulo de pascal até a linha n pode ser escrito em uma
	matriz simetrica nxn. No caso a matriz eh 4x4.
*/
const GLfloat COMB[4][4] = {
	1, 1, 1, 1,
	1, 1, 2, 3,
	1, 2, 1, 3,
	1, 3, 3, 1
};


/**
	Calcula um ponto especifico da superficie de bezier para os atributos u e v informados.
	Parametros:
		control: matriz de pontos de controle.
		u: valor do parametro u do ponto a ser gerado.
		v: valor do parametro v do ponto a ser gerado.
		xyz: array do ponto resultante, ou seja, onde o calculo de cada coordenada
		   do ponto final sera armazenada. 
*/
void getBezierSurfPoint(GLfloat control[4][4][3], GLfloat u, GLfloat v, GLfloat xyz[3]){
	xyz[0] = 0.0f;
	xyz[1] = 0.0f;
	xyz[2] = 0.0f;
	for (GLint i = 0; i < 4; i++) {
		GLfloat Jin = pow(u, (GLfloat)i) * pow(1.0f-u, 3.0f-i) * COMB[i][3];
		for (GLint j = 0; j < 4; j++){
			GLfloat Jjn = pow((GLfloat)v, (GLfloat)j) * pow(1.0f-v, 3.0f-j) * COMB[j][3];
			GLfloat J = Jin * Jjn;
			xyz[0] += control[i][j][0] * J;
			xyz[1] += control[i][j][1] * J;
			xyz[2] += control[i][j][2] * J;
		}
	}
}

/**
Calcula aproximadamente N pontos da superficie de bezier e os retorna como 
uma lista (vector<GLfloat>) chamada points de numeros reais tal que
	points[0] contem a coordenada x do primeiro ponto;
	points[1] contem a coordenada y do primeiro ponto;
	points[2] contem a coordenada z do primeiro ponto;
	...
	points[i * 3 + 0] contem a coordenada x do i-esimo ponto;
	points[i * 3 + 1] contem a coordenada y do i-esimo ponto;
	points[i * 3 + 2] contem a coordenada z do i-esimo ponto;
	...

*/
vector<GLfloat> genBezierSurfAsPoints(GLfloat control[4][4][3], GLuint N=200){
	GLfloat step = 1.0/N; //densidade de pontos em cada direcao.
	vector<GLfloat> points;
	for (GLfloat u = 0; u <= 1.0; u += step) {
		for (GLfloat v = 0; v <= 1.0; v += step) {
			GLfloat p[3];
			getBezierSurfPoint(control, u, v, p); //aplica a equacao da superficie de bezier
			points.push_back(p[0]);
			points.push_back(p[1]);
			points.push_back(p[2]);
		}
	}
	return points;
}

/**
	Tem objetivo parecido com o objetivo da funcao genBezierSurfAsPoints, exceto que
	para cada ponto gera um quadrado de tamanho 1/N para cada ponto na superficie.
	A vantagem eh que eh preciso gerar menos pontos para se obter uma visualizacao
	interessante da superficie. Com a lista de pontos gerados, pode-se
	usar a primitiva GL_QUADS da OpenGL para renderizar a superficie.
*/
vector<GLfloat> genBezierSurfAsQuads(GLfloat control[4][4][3], GLuint N=200){
	GLfloat step = 1.0/N;
	vector<GLfloat> points;
	GLfloat u = 0.0f, v = 0.0f;
	while (u <= 1.0f) {
		v = 0.0f;
		while (v <= 1.0f) {
			GLfloat p1[3], p2[3], p3[3], p4[3];
			getBezierSurfPoint(control, u, v, p1);
			getBezierSurfPoint(control, u+step, v, p2);
			getBezierSurfPoint(control, u+step, v+step, p3);
			getBezierSurfPoint(control, u, v+step, p4);
			points.push_back(p1[0]);
			points.push_back(p1[1]);
			points.push_back(p1[2]);
			points.push_back(p2[0]);
			points.push_back(p2[1]);
			points.push_back(p2[2]);
			points.push_back(p3[0]);
			points.push_back(p3[1]);
			points.push_back(p3[2]);
			points.push_back(p4[0]);
			points.push_back(p4[1]);
			points.push_back(p4[2]);
			v += step;
		}
		u += step;
	}
	return points;
}

/*
	Um facilitador paras duas funcoes anteriores. Permite ao usuario escolher o tipo de lista de
	pontos de acordo com o parametro primitive. Apenas dois valores de primitive sao permitidos:
	GL_POINTS e GL_QUADS. Se vc passar algo diferente, os pontos vao ser gerados com a funcao
	genBezierSurfAsQuads.
*/
vector<GLfloat> genBezierSurf(GLfloat control[4][4][3], GLuint N=200, GLenum primitive=GL_POINTS){
	if (primitive==GL_POINTS){
		return genBezierSurfAsPoints(control, N);
	} else {
		return genBezierSurfAsQuads(control, N);
	}
}



//Ponteiro para objeto que faz a comunicacao entre C++ e os shaders
ShaderProxy *proxy;


//INICIO DAS FUNCOES DE MAPEAMENTO ENTRE COORDENADAS DO UNIVERSO E COORDENADAS DA TELA
GLuint toXScr(GLfloat x, GLfloat minXSRU, GLfloat maxXSRU){
	return  (GLuint)(x * width/(maxXSRU-minXSRU) + 0.5f);
}

GLuint toYScr(GLfloat y, GLfloat minYSRU, GLfloat maxYSRU) {
	return (GLuint)(y * (-height)/(maxYSRU-minYSRU) + 0.5f);
}

GLfloat toXSRU(GLuint x, GLfloat minXSRU, GLfloat maxXSRU) {
	GLfloat amp = maxXSRU - minXSRU;
	return  amp * x/width + minXSRU;
}

GLfloat toYSRU(GLuint y, GLfloat minYSRU, GLfloat maxYSRU) {
	GLfloat amp = maxYSRU - minYSRU;
	return  -(amp * y/height + minYSRU);
}
//FIM DAS FUNCOES DE MAPEAMENTO ENTRE COORDENADAS DO UNIVERSO E COORDENADAS DA TELA

//Propriedades da projecao perspectiva da camera
GLfloat fov = 90.0; //fov = field of view ou angulo de abertura da camera
GLfloat eye_position[3]; //Posicao da camera
GLfloat eye_orientation[2]; //Orientacao da camera
//MATRIZES QUE COMBINAM AS TRANSFORMACOES DE MODELO, CAMERA E PROJECAO : MODEL-VIEW-PROJECTION
GLfloat view[16]; //define a posicao e a orientacao da camera
GLfloat projection[16]; //define a projecao da camera


GLfloat matrix[16]; //contem o produto projection * view * model

/**
*
* Configuracoes iniciais da aplicacao.
*
*/
int inicializar(void)
{

	identity(projection); //inicializa a matriz de projecao como a identidade
	GLfloat aspect = (GLfloat)width/height; //calcula a razao de aspecto - relacao entre largura e altura da tela
	/*
		cria uma matriz de projecao em perspectiva com razao de aspecto,
		field of view (fov) e plano de corte mais proximo de 0.001f e o 
		plano de corte mais distante igual a 100.0. Os planos de corte
		limitam o volume de visualizacao da cena (o que pode ser visto pelo observador).
		O que estiver a uma distancia menor do que 0.001 e a uma distancia maior do que
		100 em relacao ao observador (eixo -z do observador) eh eliminado do processo
		de renderizacao.
	*/	
	perspective(projection, fov, aspect, 0.001f, 10000.0f); 
	//ortho(projection, -0.5f, 0.5f, -0.5f, 0.5f, 0.001, 100.0f);
	identity(matrix);
	identity(view);
	rotationXMatrix4(matrix, viewrotx);
	rotationYMatrix4(matrix, viewroty);
	translationMatrix4(matrix, dx, dy, -0.2f);
	
	//PONTOS DE CONTROLE DA SUPERFICIES DE BEZIER
	GLfloat ctl[4][4][3] = {
							{ 	{-0.2f, 0.0f, 0.0f}, {0.0f, 0.2f, 0.0f}, {0.2f, 0.2f, 0.0f}, {0.4f, 0.0f, 0.0f} },
							{ 	{-0.2f, 0.2f, -0.2f}, {0.0f, 0.4f, -0.2f}, {0.2f, 0.4f, -0.2f}, {0.4f, 0.2f, -0.2f} },
							{ 	{-0.2f, 0.2f, -0.5f}, {0.0f, 0.4f, -0.5f}, {0.2f, 0.4f, -0.5f}, {0.4f, 0.2f, -0.5f} },
							{ 	{-0.2f, 0.0f, -0.8f}, {0.0f, 0.2f, -0.8f}, {0.2f, 0.2f, -0.8f}, {0.4f, 0.0f, -0.8f} } };

	std::vector<GLfloat> surf = genBezierSurf(ctl, 20, GL_QUADS);

	objetos.push_back(new Objeto(OBJ_GENERIC, surf, 2.0f, GL_QUADS));

	glEnable(GL_POINT_SMOOTH); //suavise as bordas do ponto
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE); //permite alterar o tamanho dos pontos no vertex shader

	try {
		program = genProgram("shader.vs", "shader.fs");
		proxy = new ShaderProxy(program);
	} catch(string error) {
		cout<<error<<endl;
		return 0;
	}
	return 1;
}


void carregarObjeto(vector<GLfloat> &objeto, GLfloat pointSize = 1.0f, GLuint coordenadas=3){
	proxy->useProgram();
	proxy->setAttribute("posicao", &objeto[0], objeto.size() * sizeof(GLfloat), 
								coordenadas);
	proxy->setUniform4f("cor", 1.0f, 0.0f, 0.0f, 0.0f);
	proxy->setUniform1f("pointSize", pointSize);
	proxy->setUniformMatrix4fv("projecao", projection, 1, GL_TRUE);
	proxy->setUniformMatrix4fv("modelo", matrix, 1, GL_TRUE);
	proxy->setUniformMatrix4fv("camera", view, 1, GL_TRUE);
}

/**
*
* Atualiza o desenho na tela de acordo com o modelo de cena atual.
*
*/
void atualizarTela()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (proxy) {
		try {
			proxy->useProgram();
			for (int i = 0; i < objetos.size(); i++){
				vector<GLfloat> objeto = objetos[i]->pontos;
				carregarObjeto(objeto, objetos[i]->pointSize, objetos[i]->nb_coordinates);
				proxy->drawArrays(objetos[i]->primitive);
			}
		} catch(string error) {
			cout<<error<<endl;
		}
	} 
	glutSwapBuffers();
}

void tratarEventoDoMouse(int button, int state, int x, int y){
		cout<<"Mouse state is "<< ( state == GLUT_DOWN ?  "DOWN" : "UP" ) << endl; 
		cout<<"Mouse event on ("<<x<<","<<y<<") position of the screen!"<<endl; 
		GLfloat xw = toXSRU(x, -1.0f, 1.0f);
		GLfloat yw = toYSRU(y, -1.0f, 1.0f);
		
		if (state == GLUT_DOWN) {
			
		} else if (state == GLUT_UP) {
			selected = NULL;
		}
}

void movimentoDoMouse(int x, int y) {
	//Tratar eventos de movimento de mouse.
}

/**
 Permite manipulacao por meio do teclado do objeto mostrado na tela.
*/
void tratarEventoDoTeclado(unsigned char key, int x, int y){
	cout<<"Key pressed: "<<key<<endl;
	cout<<"Point: ("<<x<<", "<<y<<")"<<endl;

	if (key =='x') {
		viewrotx += 1;
	} else if (key == 'X') {
		viewrotx -= 1;
	} else if (key =='y') {
		viewroty += 1;
	} else if (key == 'Y') {
		viewroty -= 1;
	} else if (key =='z') {
		viewrotz += 1;
	} else if (key == 'Z') {
		viewrotz -= 1;
	} else if (key == 'a'){
		dx -= 0.2f;
	} else if (key == 'd') {
		dx += 0.2f;
	} else if (key == 'w') {
		dz -= 0.2f;
	} else if (key == 's') {
		dz += 0.2f;
	}



	identity(matrix);
	rotationXMatrix4(matrix, viewrotx);
	rotationYMatrix4(matrix, viewroty);
	rotationZMatrix4(matrix, viewrotz);
	translationMatrix4(matrix, dx, dy, dz);
	proxy->setUniformMatrix2fv("modelo", matrix);	

	glutPostRedisplay();
}

/**
*
* Programa memory desalocation.
*
*/
void finalizar()
{
  glDeleteProgram(program);
  if (proxy != 0) {
	  proxy->close();
	  delete proxy;
	  proxy = 0;
  }
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
* Ponto de entrada do programa.
*
*/
int main(int argc, char* argv[])
{
	/* Funções necessárias para iniciar a GLUT */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(width, height);
	mainwindow = glutCreateWindow("Movimentando Objetos");

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
		glutDisplayFunc(atualizarTela);
		glutMouseFunc(tratarEventoDoMouse);
		glutMotionFunc(movimentoDoMouse);
		glutKeyboardFunc(tratarEventoDoTeclado);
		glutMainLoop();
	}

	/* Se o ocasionalmente programa sair ,
	liberamos os recursos da memória e completaremos ele com sucesso*/
	return EXIT_SUCCESS;
}

/**
* O arquivo scene.hpp contém código que facilita 
* a criação de objetos gráficos. Todo o código
* está dentro do espaço de nome <code>scene</code>.
*/
#pragma once

#include <iostream>
#include "shaderutils.hpp"
#include "matrixmath.hpp"

using namespace std;
using namespace shaderutils;
using namespace matrixmath;

/**
* Este espáco de nome contém funcões e classes que representam
* objetos de alto nível para concepção de programas gráficos.
*/
namespace scene {

	/**
	* A classe Object representa objetos gráficos que possuem
	* uma transformação geométrica associada e informações sobre
	* o material do objeto, como cor da reflexão difusa, cor da reflexão da
	* da luz ambiente e cor da reflexão especular. O atributo shininess define
	* como o brilho se espalha na superfície do objeto. Apesar dessa interpretação,
	* o objeto deve ser a associado a um vertex shader e um fragment shader que
	* façam a implementação de um modelo de iluminação adequado.
	*/
	class Object {
		public:
			GLfloat transform[16]; /** Array de 16 elementos para preencherem uma matriz ordenada por linha. */
			GLfloat diffuseReflection[4];  /** Cor da reflexão difusa do objeto. */ 
			GLfloat ambientReflection[4]; /** Cor da reflexão ambiente do objeto. */
			GLfloat specularReflection[4]; /** Cor da reflexão especular do objeto. */
			GLint shininess; /** Intensidade do brilho da reflexão especular. */

			/**
			* Construtor padrão.
			*/
			Object() {
				identity(transform);
				shininess = 1;
			}


			/**
			* Construtor por cópia.
			* @param origin objeto do qual a cópia será feita.
			*/
			Object(const Object &origin) {
				this->transform[0] = origin.transform[0]; this->transform[1] = origin.transform[1];
				this->transform[2] = origin.transform[2]; this->transform[3] = origin.transform[3];
				this->transform[4] = origin.transform[4]; this->transform[5] = origin.transform[5];
				this->transform[6] = origin.transform[6]; this->transform[7] = origin.transform[7];
				this->transform[8] = origin.transform[8]; this->transform[9] = origin.transform[9];
				this->transform[10] = origin.transform[10]; this->transform[11] = origin.transform[11];
				this->transform[12] = origin.transform[12]; this->transform[13] = origin.transform[13]; 
				this->transform[14] = origin.transform[14]; this->transform[15] = origin.transform[15]; 

				this->diffuseReflection[0] = origin.diffuseReflection[0];	this->diffuseReflection[1] = origin.diffuseReflection[1];
				this->diffuseReflection[2] = origin.diffuseReflection[2]; this->diffuseReflection[3] = origin.diffuseReflection[3];
			
			
				this->ambientReflection[0] = origin.ambientReflection[0];	this->ambientReflection[1] = origin.ambientReflection[1];
				this->ambientReflection[2] = origin.ambientReflection[2];this->ambientReflection[3] = origin.ambientReflection[3];


				this->specularReflection[0] = origin.specularReflection[0];	this->specularReflection[1] = origin.specularReflection[1];
				this->specularReflection[2] = origin.specularReflection[2];	this->specularReflection[3] = origin.specularReflection[3];
				this->shininess = origin.shininess;
			}

			/**
			* Define a intensidade da reflexão difusa.
			* @param R intensidade de reflexão da cor vermelha (RED).
			* @param G intensidade de reflexão da cor verde (GREEN).
			* @param B intensidade da reflexão da cor azul (BLUE).
			* @param A valor da componente alfa.
			*/
			void setDiffuseReflection(GLfloat R, GLfloat G, GLfloat B, GLfloat A) {
				diffuseReflection[0] = R;
				diffuseReflection[1] = G;
				diffuseReflection[2] = B;
				diffuseReflection[3] = A;
			}

			/**
			* Define a intensidade da reflexão ambiente.
			* @param R intensidade de reflexão da cor vermelha (RED).
			* @param G intensidade de reflexão da cor verde (GREEN).
			* @param B intensidade da reflexão da cor azul (BLUE).
			* @param A valor da componente alfa.
			*/
			void setAmbientReflection(GLfloat R, GLfloat G, GLfloat B, GLfloat A) {
				ambientReflection[0] = R;
				ambientReflection[1] = G;
				ambientReflection[2] = B;
				ambientReflection[3] = A;
			}

			/**
			* Define a intensidade da reflexão especular.
			* @param R intensidade de reflexão da cor vermelha (RED).
			* @param G intensidade de reflexão da cor verde (GREEN).
			* @param B intensidade da reflexão da cor azul (BLUE).
			* @param A valor da componente alfa.
			* @param shininess Coeficiente de especularidade.
			*/
			void setSpecularReflection(GLfloat R, GLfloat G, GLfloat B, GLfloat A, GLint shininess){
				specularReflection[0] = R;
				specularReflection[1] = G;
				specularReflection[2] = B;
				specularReflection[3] = A;
				this->shininess = shininess;
			}

			/**
			* Define o valor do coeficiente de especularidade.
			* @param s Coeficiente de especularidade.
			*/
			void setShininess(GLint s) {
				this->shininess = s;
			}

			/**
			* Desloca o objeto nas direções <code>x</code>,
			* <code>y</code> e <code>z</code>.
			* @param x distância do deslocamento ao longo do eixo x.
			* @param y distância do deslocamento ao longo do eixo y.
			* @param z distância do deslocamento ao longo do eixo z.
			*/
			void translate(GLfloat x, GLfloat y, GLfloat z) {
				GLfloat translation[16];
				identity(translation);
				translationMatrix4(translation, x, y, z);
				multMatrix4(this->transform,  translation, this->transform);
			}

			/**
			* Rotaciona o objeto em torno do eixo X em um ângulo de Euler
			* igual ao valor no parâmetro <code>ang</code>.
			* @param ang ângulo de rotação em graus.
			*/
			void rotateX(GLfloat ang) {
				GLfloat t[16];
				rotationXMatrix4(t, ang);
				multMatrix4(transform, t, transform);
			}

			/**
			* Rotaciona o objeto em torno do eixo Y em um ângulo de Euler
			* igual ao valor no parâmetro <code>ang</code>.
			* @param ang ângulo de rotação em graus.
			*/
			void rotateY(GLfloat ang) {
				GLfloat t[16];
				rotationYMatrix4(t, ang);
				multMatrix4(transform, t, transform);
			}

			void getPosition(GLfloat pos[]){
				pos[0] = this->transform[3];
				pos[1] = this->transform[7];
				pos[2] = this->transform[11];
			}

			/**
			* Rotaciona o objeto em torno do eixo Z em um ângulo de Euler
			* igual ao valor no parâmetro <code>ang</code>.
			* @param ang ângulo de rotação em graus.
			*/
			void rotateZ(GLfloat ang) {
				GLfloat t[16];
				rotationZMatrix4(t, ang);
				multMatrix4(transform, t, transform);
			}


			/**
			* Verifica se dois objetos são os mesmos.
			* @param origin Objeto a ser comparado.
			* @return bool true se os dois objetos são os mesmos ou false caso contrário.
			*/
			bool operator ==(const Object &origin) {
				return  (&origin) == this;
			}


			/**
			* Atribui as configurações de transformação do objeto <code>origin</code>
			* ao objeto que recebe a atribuição.
			* @return Object& referência ao objeto recebedor da atribuição.
			*/
			Object& operator =(const Object &origin) {
				if (this != &origin) {
					this->transform[0] = origin.transform[0]; this->transform[1] = origin.transform[1];
					this->transform[2] = origin.transform[2]; this->transform[3] = origin.transform[3];
					this->transform[4] = origin.transform[4]; this->transform[5] = origin.transform[5];
					this->transform[6] = origin.transform[6]; this->transform[7] = origin.transform[7];
					this->transform[8] = origin.transform[8]; this->transform[9] = origin.transform[9];
					this->transform[10] = origin.transform[10]; this->transform[11] = origin.transform[11];
					this->transform[12] = origin.transform[12]; this->transform[13] = origin.transform[13]; 
					this->transform[14] = origin.transform[14]; this->transform[15] = origin.transform[15]; 

					this->diffuseReflection[0] = origin.diffuseReflection[0];	this->diffuseReflection[1] = origin.diffuseReflection[1];
					this->diffuseReflection[2] = origin.diffuseReflection[2];
				
					this->ambientReflection[0] = origin.ambientReflection[0];	this->ambientReflection[1] = origin.ambientReflection[1];
					this->ambientReflection[2] = origin.ambientReflection[2];

					this->specularReflection[0] = origin.specularReflection[0];	this->specularReflection[1] = origin.specularReflection[1];
					this->specularReflection[2] = origin.specularReflection[2];
					this->shininess = origin.shininess;
				}
				return *this;
			}

			/**
			* Realiza um redimensionamento no objeto correspondente aos valores
			* <code>x</code>, <code>y</code> e <code>z</code>.
			* @param x Aumento de escala ao longo do eixo x.
			* @param y Aumento de escala ao longo do eixo y.
			* @param z Aumento de escala ao longo do eixo z.
			*/
			void scale(GLfloat x, GLfloat y, GLfloat z) {
				GLfloat t[16];
				identity(t);
				scaleMatrix4(t, x, y, z);
				multMatrix4(this->transform, t, this->transform);
			}

			/**
			* Aplica uma transformação genérica representada pela
			* matriz <code>matrix</code>. 
			* @param matrix um array de <code>16</code> elementos
			* representando uma matriz 4x4. A ordenação do array é feita por linha da
			* matrix. Assim, os quatro primeiros elementos representam a primeira linha,
			* os quatro segundos elementos, a segunda linha; e assim por diante.
			*/
			void applyTransform(GLfloat matrix[16]) {
				multMatrix4(transform, matrix , transform);
			}
	};


	/**
	* A classe ShadedObject representa objetos que podem ser associados um 
	* programa GLSL (que também chamamos de shader). Assim, um objeto do tipo
	* ShadedObject combina um objeto do tipo Object a um objeto ShaderProxy. 
	* Isso permite que um objeto do tipo Object possa ser renderizado. Observe
	* que a combinação dos elementos deve ser feita de forma coerente. Todas as 
	* propriedades declaradas em ShadedObject devem ter variáveis correspondentes
	* no programa GLSL e um modelo coerente de processamento dos dados geométricos,
	* topológicos e do modelo de iluminação ou de colorização subjacente.
	*
	*/
	class ShadedObject {
		private:
			string name; /** Nome do objeto. Útil para fins de depuração. */

		public:
			Object data; /** Dados geométricos e de material do objeto. */
			ShaderProxy *shader; /** Um proxy para um programa GLSL. */
			string transformName; /**  Nome do uniforme do tipo mat4 que conterá a transformação desse objeto. Essa variável fica no programa GLSL associado. */
			string diffuseReflectionName; /** No programa GSLS associado, é o nome da variável  do tipo vec4 que conterá os valores da reflexão difusa do material do objeto. */
			string ambientReflectionName; /** No programa GLSL associado, é o nome da variável do tipo vec4 que conterá os valores da reflexão ambiente do material do objeto. */
			string specularReflectionName; /** No programa GLSL associado, é o nome da variável do tipo vec4 que conterá os valores da reflexão especular do material do objeto. */
			string shininessName; /** No programa GLSL associado, é o nome da variável do tipo float  que será o coeficiente de especularidade do modelo de reflexão de Phong. */

			/**
			* Cria um objeto com nome cujo valor está no parâmetro <code>name</code>
			* e cujo objeto <code>ShaderProxy</code> associado é apontado pelo ponteiro <code>proxy</code>.
			*
			*/
			ShadedObject(string name, ShaderProxy *proxy) {
				this->name = name;
				this->shader = proxy;
				transformName = "model";
				diffuseReflectionName = "diffuseReflection";
				ambientReflectionName = "ambientReflection";
				specularReflectionName = "specularReflection";
				shininessName = "shininess";
			}

			/**
			* Define o objeto de dados (geométricos e de material) associados ao proxy deste objeto.
			* @param data Referência para o objeto de dados (do tipo Object) a ser associado ao objeto do tip ShaderProxy deste objeto.
			*/
			void setData(const Object &data) {
				this->data = data;
				update();
			}

			/**
			* Toda vez que uma informação dos objetos associados (ShaderProxy ou Object)
			* for alterada, execute o método update de ShadedProxy.
			*/
			void update() {

				try {
					this->shader->setUniformMatrix4fv(transformName.c_str(), data.transform, 1, GL_TRUE);
				} catch(string error) {
					cout<<error<<endl;
				}

				try {
					this->shader->useProgram();

					this->shader->setUniform4f(diffuseReflectionName.c_str(), data.diffuseReflection[0],
														data.diffuseReflection[1], data.diffuseReflection[2], 
														data.diffuseReflection[3]);
					
					this->shader->setUniform4f(ambientReflectionName.c_str(), data.ambientReflection[0],
														data.ambientReflection[1], data.ambientReflection[2],
														data.ambientReflection[3]);
					
					this->shader->setUniform4f(specularReflectionName.c_str(), data.specularReflection[0],
														data.specularReflection[1], data.specularReflection[2],
														data.specularReflection[3]);

					this->shader->setUniform1i(shininessName.c_str(), data.shininess);
				} catch(string error){
					cout<<error<<endl;
				}
			}

			/**
			* Desenha o objeto atual de acordo com o ShaderProxy e o Object associados.
			*/
			void drawElements() {
				shader->useProgram();
				shader->drawElements();
			}

			/**
			* Desenha o objeto de acordo com o ShaderProxy e o Object associados, possibilitando
			* definir a primitiva de desenho da OpenGL.
			* @param primitive É um valor que define a primitiva de desenho. Pode ser, por exemplo, GL_TRIANGLES, GL_QUADS, GL_LINES, dentre outras.
			*/
			void drawArrays(GLenum primitive=GL_QUADS){
				shader->useProgram();
				shader->drawArrays(primitive);
			}
	};


	/**
	* Uma classe que permite criar objetos que encapsulam informações de câmetra, como
	* posição, orientação e tipos de projeção.
	*/
 	class Camera {

 		public:
 			GLfloat projection[16]; /** Matriz de projeção. */
 			GLfloat view[16]; /** Matriz view, que permite definir posição e orientação da câmera. */
 			ShaderProxy *shader; /** ShaderProxy que deverá ter o programa GLSL que utilizará a câmera. */
 			string viewName; /** No programam GLSL associado via ShaderProxy, o nome da variável do tipo mat4 que contém a matriz view. */
 			string projectionName; /** No programam GLSL associado via ShaderProxy, o nome da variável do tipo mat4 que contém a matriz projection. */

 			/**
 			* Cria um câmera associada com um programa GLSL representado pelo ShaderProxy
 			* apontado pelo ponteiro <code>shader</code>.
 			* @param shader ponteiro para o ShaderProxy que conterá a câmera.
 			*/
 			Camera(ShaderProxy *shader) {
 				this->shader = shader;
 				this->viewName = "view";
 				this->projectionName = "projection";
 				this->setViewIdentity();
 				this->setProjectionIdentity();
 			}

			/**
			* Desloca a câmera nas direções <code>x</code>,
			* <code>y</code> e <code>z</code>.
			* @param x distância do deslocamento ao longo do eixo x.
			* @param y distância do deslocamento ao longo do eixo y.
			* @param z distância do deslocamento ao longo do eixo z.
			*/
 			void translate(GLfloat dx, GLfloat dy, GLfloat dz) {
 				GLfloat t[16];
 				identity(t);
 				translationMatrix4(t, dx, dy, dz);
 				multMatrix4(t, view, view);
 			}

			/**
			* Rotaciona a câmera em torno do eixo X em um ângulo de Euler
			* igual ao valor no parâmetro <code>ang</code>.
			* @param ang ângulo de rotação em graus.
			*/
 			void rotateX(GLfloat ang) {
				GLfloat t[16];
				identity(t);
 				rotationXMatrix4(t, ang);
 				multMatrix4(t, view, view);
 			}
 			
 			/**
			* Rotaciona a câmera em torno do eixo Y em um ângulo de Euler
			* igual ao valor no parâmetro <code>ang</code>.
			* @param ang ângulo de rotação em graus.
			*/
 			void rotateY(GLfloat ang) {
				GLfloat t[16];
				identity(t);
 				rotationYMatrix4(t, ang);
 				multMatrix4(t, view, view);
 			}


			/**
			* Rotaciona a câmera em torno do eixo Z em um ângulo de Euler
			* igual ao valor no parâmetro <code>ang</code>.
			* @param ang ângulo de rotação em graus.
			*/
 			void rotateZ(GLfloat ang) {
				GLfloat t[16];
				identity(t);
 				rotationZMatrix4(t, ang);
 				multMatrix4(t, view, view);
 			}

 			/**
 			* Após executar esta função, matrix view que representa
 			* a posição e a orientação da câmera é resetada para a matrix identidade.
 			*/
 			void setViewIdentity() {
 				identity(view);
 			}


 			/**
 			* Após executar esta função, matrix projection que representa
 			* a posição e a orientação da câmera é resetada para a matrix identidade.
 			*/
 			void setProjectionIdentity() {
 				identity(projection);
 			}

 			/**
 			* Cria a matrix projeção perspectiva com os parâmetros abaixo relacionados.
 			* O volume de visão resultante é uma pirâmide invertida de ponta cortada delimimtada
 			* pelos planos z=near e z=far.
 			* @param fov Field Of View ou campo de vista da câmera.
 			* @param aspectRatio é a razão de aspecto.
 			* @param near é o plano mais próximo da câmera após o qual os objetos visíveis devem ser colocados.
 			* @param far é o plano mais distante da câmera após o qual os objetos visíveis devem ser colocados.
 			*/
 			void setPerspective(GLfloat fov, GLfloat aspectRatio, GLfloat near, GLfloat far) {
 				perspective(this->projection, fov, aspectRatio, near, far);
 			}

 			/**
 			* Define uma projeção ortográfica. O volume de visão correspondente é um 
 			* um paralepípedo definido de acordo com os parâmetros abaixo relacionados.
 			* @param left plano x=left que delimita o volume de visualização à esquerda.
 			* @param right plano x=right que delimita o volume de visualização à direita.
 			* @param bottom plano y=bottom que delimita o piso do volume de visulização.
 			* @param top plano y=top que delimita o topo do volume de visualização.
 			* @param near plano z=near, o plano mais próximo da câmera.
 			* @param far palno z=far, o plano mais distante da câmera.
 			*/
 			void setOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far) {
 				ortho(this->projection, left, right, bottom, top, near, far);
 			}
 			
 			/**
 			* Recupera a posição da câmera. 
 			* @param pos array que conterá a posição da câmera após a chamada da função.
 			*/
 			void getPosition(GLfloat pos[]){
 				pos[0] = this->view[3];
 				pos[1] = this->view[7];
 				pos[2] = this->view[11];
 			}

 			/**
 			* Após ser realizada uma transformação na matriz view da câmera,
 			* execute esta função para atualizar o valor da variável uniforme GLSL correspondente.
 			*/
 			void updateView() {
 				shader->setUniformMatrix4fv(viewName.c_str(), this->view, 1, GL_TRUE);
 			}

 			/**
 			* Após ser realizada uma transformação na matriz projection da câmera,
 			* execute esta função para atualizar o valor da variável uniforme GLSL correspondente.
 			*/
 			void updateProjection() {
 				shader->setUniformMatrix4fv(projectionName.c_str(), this->projection, 1, GL_TRUE);
 			}


 			/**
 			* Após ser realizada uma transformação em alguma matriz da câmera,
 			* execute esta função para atualizar o valor da variável uniforme GLSL correspondente.
 			*/
 			void update() {
 				try {
 					updateView();
 				} catch(string error) {
 					cout<<error<<endl;
 				}
 				try {
 					updateProjection();
 				} catch(string error) {
 					 cout<<error<<endl;
 				}
 			}
 	};
}

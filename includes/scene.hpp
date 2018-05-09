#pragma once

#include <iostream>
#include "shaderutils.hpp"
#include "matrixmath.hpp"

using namespace std;
using namespace shaderutils;
using namespace matrixmath;

namespace scene {
	class Object {
		public:
			GLfloat transform[16];
			GLfloat diffuseReflection[4];
			GLfloat ambientReflection[4];
			GLfloat specularReflection[4];
			GLint shininess;

			Object() {
				identity(transform);
				shininess = 1;
			}

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

			void setDiffuseReflection(GLfloat R, GLfloat G, GLfloat B, GLfloat A) {
				diffuseReflection[0] = R;
				diffuseReflection[1] = G;
				diffuseReflection[2] = B;
				diffuseReflection[3] = A;
			}

			void setAmbientReflection(GLfloat R, GLfloat G, GLfloat B, GLfloat A) {
				ambientReflection[0] = R;
				ambientReflection[1] = G;
				ambientReflection[2] = B;
				ambientReflection[3] = A;
			}

			void setSpecularReflection(GLfloat R, GLfloat G, GLfloat B, GLfloat A, GLint shininess){
				specularReflection[0] = R;
				specularReflection[1] = G;
				specularReflection[2] = B;
				specularReflection[3] = A;
				this->shininess = shininess;
			}

			void setShininess(GLint s) {
				this->shininess = s;
			}

			void translate(GLfloat x, GLfloat y, GLfloat z) {
				GLfloat translation[16];
				translationMatrix4(translation, x, y, z);
				multMatrix4(transform,  translation, transform);
			}

			void rotateX(GLfloat ang) {
				GLfloat t[16];
				rotationXMatrix4(t, ang);
				multMatrix4(transform, t, transform);
			}

			void rotateY(GLfloat ang) {
				GLfloat t[16];
				rotationYMatrix4(t, ang);
				multMatrix4(transform, t, transform);
			}

			void rotateZ(GLfloat ang) {
				GLfloat t[16];
				rotationZMatrix4(t, ang);
				multMatrix4(transform, t, transform);
			}

			bool operator ==(const Object &origin) {
				return  (&origin) == this;
			}

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

			void scale(GLfloat x, GLfloat y, GLfloat z) {
				GLfloat t[16];
				scaleMatrix4(t, x, y, z);
				multMatrix4(transform, t, transform);
			}

			void applyTransform(GLfloat matrix[16]) {
				multMatrix4(transform, matrix , transform);
			}
	};


	class ShadedObject {
		private:
			string name;

		public:
			Object data;
			ShaderProxy *shader;
			string transformName;
			string diffuseReflectionName;
			string ambientReflectionName;
			string specularReflectionName;
			string shininessName;

			ShadedObject(string name, ShaderProxy *proxy) {
				this->name = name;
				this->shader = proxy;
				transformName = "model";
				diffuseReflectionName = "diffuseReflection";
				ambientReflectionName = "ambientReflection";
				specularReflectionName = "specularReflection";
				shininessName = "shininess";
			}

			void setData(const Object &data) {
				this->data = data;
				update();
			}

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

			void drawElements() {
				shader->useProgram();
				shader->drawElements();
			}

			void drawArrays(GLenum primitive=GL_QUADS){
				shader->useProgram();
				shader->drawArrays(primitive);
			}
	};

 	class Camera {
 		private:

 			GLfloat projection[16];
 			GLfloat view[16];
 		public:
 			ShaderProxy *shader;
 			string viewName;
 			string projectionName;

 			Camera(ShaderProxy *shader) {
 				this->shader = shader;
 				this->viewName = "view";
 				this->projectionName = "projection";
 				this->setViewIdentity();
 				this->setProjectionIdentity();
 			}

 			void translate(GLfloat dx, GLfloat dy, GLfloat dz) {
 				GLfloat t[16];
 				translationMatrix4(t, dx, dy, dz);
 				multMatrix4(t, view, view);
 			}

 			void rotateX(GLfloat ang) {
				GLfloat t[16];
 				rotationXMatrix4(t, ang);
 				multMatrix4(t, view, view);
 			}
 			
 			void rotateY(GLfloat ang) {
				GLfloat t[16];
 				rotationYMatrix4(t, ang);
 				multMatrix4(t, view, view);
 			}

 			void rotateZ(GLfloat ang) {
				GLfloat t[16];
 				rotationZMatrix4(t, ang);
 				multMatrix4(t, view, view);
 			}

 			void setViewIdentity() {
 				identity(view);
 			}


 			void setProjectionIdentity() {
 				identity(projection);
 			}

 			void setPerspective(GLfloat fov, GLfloat aspectRatio, GLfloat near, GLfloat far) {
 				perspective(this->projection, fov, aspectRatio, near, far);
 			}

 			void setOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far) {
 				ortho(this->projection, left, right, bottom, top, near, far);
 			}
 			
 			void getPosition(GLfloat pos[]){
 				pos[0] = this->view[3];
 				pos[1] = this->view[7];
 				pos[2] = this->view[11];
 			}

 			void updateView() {
 				shader->setUniformMatrix4fv(viewName.c_str(), this->view, 1, GL_TRUE);
 			}

 			void updateProjection() {
 				shader->setUniformMatrix4fv(projectionName.c_str(), this->projection, 1, GL_TRUE);
 			}

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

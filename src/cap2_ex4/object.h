#pragma once

#include "shader.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/trigonometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <list>
#include <string>
#include <iostream>

using namespace std;

class Scene;

class Camera {
public:
	glm::mat4 projection;
	
	glm::mat4 view;
	
	Camera() {

	}


	glm::mat4 getTransform() const {
		return (projection * view);
	}
};

class GraphicObject {
private:
	GLfloat *vertices;
	GLfloat *colors;
	GLfloat *normals;
	GLuint *indices;
	GLsizei numberOfVertices;

public:
	glm::mat4 transform;
	Shader *shader;
	GLenum primitive;
	Attribute colorAttribute;
	Attribute verticesAttribute;
	Attribute normalsAttribute;
	Uniform transformUniform;

	GraphicObject() {
		shader = NULL;
		vertices = NULL;
		indices = NULL;
		colors = NULL;
		indices = NULL;
		primitive = GL_TRIANGLES;
	}

	GraphicObject(Shader *shader,
								  GLfloat *vertices = NULL, GLsizei numberOfVertices=0, GLfloat *colors=NULL, 
								  GLfloat *normals=NULL, GLuint *indices=NULL, GLenum primitive = GL_TRIANGLES) {
		this->shader = shader;
		this->vertices = vertices;
		this->colors = colors;
		this->indices = indices;
		this->normals = normals;
		this->primitive = primitive;
		this->numberOfVertices = numberOfVertices;
		this->colorAttribute = Attribute("aColor");
		this->verticesAttribute = Attribute("aVertices");
		this->normalsAttribute = Attribute("aNormals");
		this->transformUniform = Uniform("uTransform");
	}

	void initShader() {
		glUseProgram(shader->getProgramID());
		shader->setUniform(transformUniform);
		if (shader->initUniform(transformUniform.getName())) {
			shader->bindUData4fv(transformUniform.getName(), 1, glm::value_ptr(transform));
		}

		if (colors) {
			shader->setAttribute(colorAttribute);
			if (shader->initAttribute(colorAttribute.getName())) {
				shader->setAttributeData(colorAttribute.getName(), colors, colorAttribute.getVertexSize() * numberOfVertices * sizeof (GLfloat) );
			}
		}


		if (normals) {
			shader->setAttribute(normalsAttribute);
			if (shader->initAttribute(normalsAttribute.getName())) {
				shader->setAttributeData(normalsAttribute.getName(), normals, normalsAttribute.getVertexSize() * numberOfVertices * sizeof (GLfloat) );	
			}
		}

		if (vertices) {
			shader->setAttribute(verticesAttribute);
			if (shader->initAttribute(verticesAttribute.getName())) {
				shader->setAttributeData(verticesAttribute.getName(), vertices, verticesAttribute.getVertexSize() * numberOfVertices * sizeof(GLfloat) );
			}
		}
	}

	virtual void drawShader(const Scene &Scene) {
		glUseProgram(shader->getProgramID());

		if (colors) {
			shader->enableAttribute(colorAttribute.getName());
		}


		if (normals) {
			shader->enableAttribute(normalsAttribute.getName());
		}

		if (vertices) {
			shader->enableAttribute(verticesAttribute.getName());
		}

		draw();

		if (colors) {
			shader->disableAttribute(colorAttribute.getName());
		}


		if (normals) {
			shader->disableAttribute(normalsAttribute.getName());
		}

		if (vertices) {
			shader->disableAttribute(verticesAttribute.getName());
		}
	}

	virtual void draw() {
		if (!indices) {
			glDrawArrays(primitive, 0, numberOfVertices );
		} else {
			glDrawElements(primitive, 0, GL_UNSIGNED_INT, indices);
		}
	}

	~GraphicObject(){
		shader = NULL;
		if (vertices != NULL) {
			vertices = NULL;
		}

		if (indices != NULL) {
			indices = NULL;
		}

		if (colors != NULL) {
			colors = NULL;
		}

		if (normals != NULL) {
			normals = NULL;
		}
	}
};

class Scene {
private:
	GLbitfield buffers;
public:
	list< GraphicObject* > objects;
	Camera camera;
	glm::vec3 background;
	
	Scene() {
		buffers = GL_COLOR_BUFFER_BIT;
		background = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	void resetTest() {
		buffers = GL_COLOR_BUFFER_BIT;
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
	}


	void resetState() {
		resetTest();
		background = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	void enableDepth() {
		glEnable(GL_DEPTH_TEST);
		buffers = buffers & GL_DEPTH_BUFFER_BIT;
	}

	void enableStencilBuffer() {
		glEnable(GL_STENCIL_TEST);
		buffers = buffers & GL_STENCIL_BUFFER_BIT;
	}

	void draw() {
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(buffers);
		for (list< GraphicObject* >::iterator it = objects.begin(); it != objects.end(); ++it) {
			GraphicObject *obj = *it;
			obj->drawShader(*this);
		}
	}
};


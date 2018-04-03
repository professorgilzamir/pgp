#pragma once

#include <string>
#include <map>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <cstdio>

using namespace std;

class Uniform {
	GLint location;
	string name;
public:
	Uniform(){
		this->location = -1;
	}
	
	Uniform(string n) {
		this->location = -1;
		this->name = n;
	}
	
	GLint getLocation() const { return location; }
	void setLocation(GLint l) {
		this->location = l;
	}
	
	
	string getName() const {
		return name;
	}
};


class Attribute {
	GLint location;
	GLenum type;
	GLenum usage;
	string name;
	GLsizei vertexSize;
	GLint vertexComponentType;
	GLuint bufferId;
public:
	Attribute() {
		this->vertexSize = 0;
		this->vertexComponentType = -1;
		this->location = -1;
		this->type = GL_ARRAY_BUFFER;
	}

	Attribute(string name, GLsizei vsize=2, GLint vertexCompType=GL_FLOAT, GLenum type=GL_ARRAY_BUFFER, 
						GLenum usage=GL_STATIC_DRAW) {
		this->name = name;
		this->type = type;
		this->usage = usage;
		this->vertexSize = vsize;
		this->vertexComponentType = vertexCompType;
		this->location = -1;
	}	

	
	GLsizei getVertexSize() const {
		return this->vertexSize;
	}
	
	GLint getVertexComponentType() const {
		return this->vertexComponentType;
	}
	
	
	const string getName()  const {
		return name;
	}
	
	
	void setLocation(GLint ref) {
		this->location = ref;
	}
	
	GLint getLocation() const {
		return this->location;
	}
	
	void createBuffer() {
		glGenBuffers(1, &bufferId);
	}
	
	void bindData(const GLvoid *data, GLsizei size) {
		setBuffer();
		glBufferData(type, size, data, usage);
  		unsetBuffer();
	}
	
	
	void setBuffer() {
		glBindBuffer(type, bufferId);
	}
	
	GLuint getBufferId() const {
		return this->bufferId;
	}
	
	void unsetBuffer() {
		glBindBuffer(type, 0);
	}
	
	bool operator ==(const Attribute &attr){
		return this->name == this->name;
	} 
};

class Shader {
private:
	GLuint program;
	map<string, Attribute> attributes;
	map<string, Uniform> uniforms;
	
public:
	Shader(const string &vertexContent, const string &fragmentContent) {
		GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;
		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		const char* vscontent = vertexContent.data();
		glShaderSource(vs, 1, &vscontent, NULL);
		glCompileShader(vs);
		glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);

		if (0 == compile_ok){
			throw getShaderLog(vs);
		}

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		const char *fscontent = fragmentContent.data();
		glShaderSource(fs, 1, &fscontent, NULL);
		glCompileShader(fs);
		glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
		if (0==compile_ok) {
			throw getShaderLog(fs);
		}

		program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &link_ok);

		if (!link_ok) {		
			getShaderLog(program);
		}
	}

	void init() {
		for (map<string, Attribute>::iterator it = attributes.begin();
			 it != attributes.end(); ++it) {
			string name = it->first;
			Attribute attr = it->second;
			GLint ref = glGetAttribLocation(program, name.data());
			attr.setLocation(ref);
			attr.createBuffer();
			attributes[name] = attr;
		}
		
		
		for (map<string, Uniform>::iterator it = uniforms.begin();
				it != uniforms.end(); ++it) {
				
				string name = it->first;
				Uniform uni = it->second;
				GLint location = glGetUniformLocation(program, name.data()); 		
				uni.setLocation(location);
				uniforms[name] = uni;	
		}
	}


	bool initAttribute(const string &name) {
		Attribute attr = attributes[name];
		if (attr.getLocation() < 0) {
			GLint ref = glGetAttribLocation(program, name.data());
			if (ref >= 0) {
				attr.setLocation(ref);
				attr.createBuffer();
				attributes[name] = attr;
			}
			return ref >= 0;
		} else {
			return true;
		}
	}

	bool initUniform(const string &name) {
			Uniform uni = uniforms[name];
			if (uni.getLocation() >= 0) {
				return true;
			}
			GLint location = glGetUniformLocation(program, name.data()); 		
			if (location) {
				uni.setLocation(location);
				uniforms[name] = uni;	
			}
			return location >= 0;
	}
	
	Attribute getAttribute(string name) {
		return attributes[name];
	}
	
	GLuint getProgramID(){
		return program;
	}
	
	void setAttribute(const Attribute &attr){
		attributes[attr.getName()] = attr;
	}
	
	void setAttributeData(string name, GLvoid *data, GLsizei size) {
		(attributes[name]).bindData(data, size);
	}
	
	void enableAttribute(string name, GLint normalized=GL_FALSE, GLsizei stride=0, 
				GLvoid *start=0) {
		Attribute attr = attributes[name];
		attr.setBuffer();
		glEnableVertexAttribArray(attr.getLocation());
	  	glVertexAttribPointer(
    		attr.getLocation(), // atributo
    		attr.getVertexSize(),                 // numero de elementos por vértices, aqui é (x,y)
    		attr.getVertexComponentType(),          // o tipo de cada elemento
    		normalized,          // Como nossos valores estão.
    		stride,                 // sem dados extras em cada posição
    		start               // descolocamento do primeiro elemento
  		); 	
	}
	
	void disableAttribute(string name) {
		glDisableVertexAttribArray(getAttribute(name).getLocation());
	}
	
	string getShaderLog(GLuint object) {
		string slog;
	 	GLint log_length = 0;
  		
  		if (glIsShader(object))
    		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  		else if (glIsProgram(object))
    		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  		

  		char* log = (char*)malloc(log_length);

  		if (glIsShader(object))
   			glGetShaderInfoLog(object, log_length, NULL, log);
  		else if (glIsProgram(object))
    		glGetProgramInfoLog(object, log_length, NULL, log);
    	
    	
    		slog.append(log);

		free(log);

		log = NULL;
	
    		return slog;
	}
	
	Uniform getUniform(string name){
		return uniforms[name];
	}
	
	void setUniform(const Uniform &uniform) {
		uniforms[uniform.getName()] = uniform;
	}
	
	void bindUData1f(string name, GLfloat v) {
		Uniform u = uniforms[name];
		glUniform1f(u.getLocation(), v);
	}
	
	void bindUData2f(string name, GLfloat v1, GLfloat v2) {
		Uniform u = uniforms[name];
		glUniform2f(u.getLocation(), v1, v2);
	}
	
	void bindUData3f(string name, GLfloat v1, GLfloat v2, GLfloat v3) {
		Uniform u = uniforms[name];
		glUniform3f(u.getLocation(), v1, v2, v3);
	}

	void bindUData4f(string name, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4) {
		Uniform u = uniforms[name];
		glUniform4f(u.getLocation(), v1, v2, v3, v4);
	}


	void bindUData1i(string name, GLint v) {
		Uniform u = uniforms[name];
		glUniform1i(u.getLocation(), v);
	}
	
	void bindUData2i(string name, GLint v1, GLint v2) {
		Uniform u = uniforms[name];
		glUniform2i(u.getLocation(), v1, v2);
	}
	
	void bindUData3i(string name, GLint v1, GLint v2, GLint v3) {
		Uniform u = uniforms[name];
		glUniform3i(u.getLocation(), v1, v2, v3);
	}

	void bindUData4i(string name, GLint v1, GLint v2, GLint v3, GLint v4) {
		Uniform u = uniforms[name];
		glUniform4i(u.getLocation(), v1, v2, v3, v4);
	}

	void bindUData1fv(string name, GLsizei count, GLfloat *v) {
		Uniform u = uniforms[name];
		glUniform1fv(u.getLocation(), count, v);
	}


	void bindUData2fv(string name, GLsizei count, GLfloat *v) {
		Uniform u = uniforms[name];
		glUniform2fv(u.getLocation(), count, v);
	}
	
	void bindUData3fv(string name, GLsizei count, GLfloat *v) {
		Uniform u = uniforms[name];
		glUniform3fv(u.getLocation(), count, v);
	}
	
	void bindUData4fv(string name, GLsizei count, GLfloat *v) {
		Uniform u = uniforms[name];
		glUniform4fv(u.getLocation(), count, v);
	}

	void bindUData1iv(string name, GLsizei count, GLint *v) {
		Uniform u = uniforms[name];
		glUniform1iv(u.getLocation(), count, v);
	}

	void bindUData2iv(string name, GLsizei count, GLint *v) {
		Uniform u = uniforms[name];
		glUniform2iv(u.getLocation(), count, v);
	}
	
	void bindUData3iv(string name, GLsizei count, GLint *v) {
		Uniform u = uniforms[name];
		glUniform3iv(u.getLocation(), count, v);
	}
	
	void bindUData4iv(string name, GLsizei count, GLint *v) {
		Uniform u = uniforms[name];
		glUniform4iv(u.getLocation(), count, v);
	}

	
	void bindUDataMatrix2fv(string name,
 						GLsizei count,
 						GLboolean transpose,
 						const GLfloat *value) {
 		GLint loc = uniforms[name].getLocation();
 		glUniformMatrix3fv(loc, count, transpose, value);
 	}
 
	void bindUDataMatrix3fv(string name,
 								GLsizei count,
 								GLboolean transpose,
 								const GLfloat *value) {
 		GLint loc = uniforms[name].getLocation();
 		glUniformMatrix3fv(loc, count, transpose, value); 								
 	}
 
	void bindUDataMatrix4fv(string name,
 								GLsizei count,
 								GLboolean transpose,
 								const GLfloat *value) {
 		GLint loc = uniforms[name].getLocation();
 		glUniformMatrix4fv(loc, count, transpose, value);						
 	}


	~Shader() {
		for (map<string, Attribute>::iterator it = attributes.begin();
			it != attributes.end(); ++it) {
			
			Attribute attr = it->second;
			GLuint id = attr.getBufferId();
			glDeleteBuffers(1, &id);
		}
		glDeleteProgram(program);
	}
};


#pragma once
#include <string>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif

#include "utils.hpp"

using namespace utils;

using namespace std;

namespace shaderutils {

    void getErrorShaderInfoLog(GLuint target, GLuint type) {
            GLint maxLength = 1000;
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(target, maxLength, &maxLength, &errorLog[0]);
            string info;
            if (type == 0) {
                info.append("Vertex Shader Compilation Error:: ");
            } else if (type == 1) {
                info.append("Fragment Shader Compilatio  Error:: ");
            } else {
                info.append("Link error:: ");
            }

            info.append(string(errorLog.begin(), errorLog.end()));
            throw info;
    }

    /**
    * Compila os dois shaders apontados pelos arquivos vsname e fsname. 
    *  A funcao compila os shaders e retorna o programa correspondente ou
    *  lanca uma exceccao cujo argumento eh uma string indicando o erro que ocorreu.
    *  A forma correta de chamar esta funcao eh:
    *
    *    try {
    *       GLuint meuprogram = genProgram("meuvertexshader.txt", "meufragmentshader.txt");
    *    } catch(string error) {
    *       cout<<"Ocorreu o seguinte erro: "<<error<<endl;
    *       //outras acoes necessarias
    *    }
    *
    *  onde
    *    "meuvertexshader.txt" eh o nome do arquivo ou path que contem o codigo GLSL correspondente ao
    *        Vertex Shader; e 
    *   "meufragmentshader.txt" eh o nome do arquivo ou path que contem o codigo GLSL que corresponde
    *        ao Fragment Shader.
    */
    GLuint genProgram(const char *vsname, const char *fsname) {
        GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        string *shadervs = readfile(vsname);
        const char* vscontent = shadervs->data();
        glShaderSource(vs, 1, &vscontent, NULL);
        glCompileShader(vs);
        glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);

        if (0 == compile_ok){
            delete shadervs;
            getErrorShaderInfoLog(vs, 0);
        }

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        string *shaderfs = readfile(fsname);
        const char *fscontent = shaderfs->data();
        glShaderSource(fs, 1, &fscontent, NULL);
        glCompileShader(fs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
        if (0==compile_ok) {
            delete shadervs;
            delete shaderfs;
            getErrorShaderInfoLog(fs, 1);
            return 0;
        }

        GLuint program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &link_ok);


	    delete shadervs;
	    delete shaderfs;

    	if (!link_ok) {
		    delete shadervs;
		    delete shaderfs;
            getErrorShaderInfoLog(program, 2);
		    return 0;
	    }

        return program;
    }

    GLuint createVertexBuffer() {
        GLuint bufferID;
        glGenBuffers(1, &bufferID);
        return bufferID;
    }

    GLuint createElementBuffer() {
        GLuint bufferID;
        glGenBuffers(1, &bufferID);
        return bufferID;
    }

    void setVertexDataBuffer(GLuint bufferID, GLfloat *values, GLuint countBytes, GLenum mode = GL_STATIC_DRAW) {
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ARRAY_BUFFER, countBytes, values, mode);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void setElementDataBuffer(GLuint bufferID, GLuint *values, GLuint countBytes, GLenum mode = GL_STATIC_DRAW) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, countBytes, values, mode);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    class ShaderProxy {

        private:
            GLuint countAttributes;
            GLuint program;

            GLint *attributeID;
            GLuint  *attributeSize;
            GLint *uniformID;
            GLuint *attributeBufferID;
            
            GLuint *indexList;
            GLenum primitive;
            GLuint countPrimitives;
        public:

            ShaderProxy(GLuint program, GLuint countAttributes, GLuint countUniforms){
                this->program = program;
                this->countAttributes = countAttributes;
                this->countUniforms = countUniforms; 
                this->attributeID = new GLint[countAttributes];
                this->attributeSize = new GLuint[countAttributes];
                this->attributeBufferID = new GLuint[countAttributes];
            }

            GLint getAttribute(GLuint idx, const char *name) {
                attributeID[idx] = glGetAttribLocation(program, name);
                return attributeID[idx];
            }

            void setElementPrimitive(GLuint *indexList, GLuint countPrimitives,
                        GLenum primitive) {
                    this->indexList = indexList;
                    this->countPrimitives = countPrimitives;
                    this->primitive;
            }

            void setAttribute(GLuint idx, GLfloat *values, GLuint countBytes, GLuint attributeSize,
                    GLenum mode = GL_STATIC_DRAW) {
                    attributeBufferID[idx] = createVertexBuffer();
                    attributeSize[idx] = attributeSize;
                    setVertexDataBuffer(attributeBufferID[idx], values, countBytes, mode);
            }


            GLint setUniform1f(const char *name, GLfloat value) {
                GLint uniformID = glGetUniformLocation(program, name); 
                glUniform1f(uniformID, value);
                return uniformID;
            }

            GLint setUniform2f(const char *name, GLfloat value1, GLfloat value2) {
                GLint uniformID = glGetUniformLocation(program, name); 
                glUniform2f(uniformID, value1, value2);
                return uniformID;
            }

            GLint setUniform3f(const char *name, GLfloat value1, GLfloat value2, GLfloat value3) {
                GLint uniformID = glGetUniformLocation(program, name); 
                glUniform3f(uniformID, value1, value2 , value3);
                return uniformID;
            }

            GLint setUniform4f(const char *name, GLfloat value1, GLfloat value2, GLfloat value3, GLfloat value4) {
                GLint uniformID = glGetUniformLocation(program, name); 
                glUniform4f(uniformID, value1, value2 , value3, value4);
                return uniformID;
            }

            GLint setUniform1ui(const char *name, GLuint value) {
                GLint uniformID = glGetUniformLocation(program, name); 
                glUniform1ui(uniformID, value);
                return uniformID;
            }

            GLint setUniform2ui(const char *name, GLuint value1, GLuint value2) {
                GLint uniformID = glGetUniformLocation(program, name); 
                glUniform2ui(uniformID, value1, value2);
                return uniformID;
            }

            GLint setUniform3ui(const char *name, GLuint value1, GLuint value2, GLuint value3) {
                GLint uniformID = glGetUniformLocation(program, name); 
                glUniform3ui(uniformID, value1, value2 , value3);
                return uniformID;
            }

            GLint setUniform4ui(const char *name, GLuint value1, GLuint value2, GLuint value3, GLuint value4) {
                GLint uniformID = glGetUniformLocation(program, name); 
                glUniform4ui(uniformID, value1, value2 , value3, value4);
                return uniformID;
            }

            GLint setUniform1i(const char *name, GLint value) {
                GLint uniformID = glGetUniformLocation(program, name); 
                glUniform1i(uniformID, value);
                return uniformID;
            }

            GLint setUniform2i(const char *name, GLint value1, GLint value2) {
                GLint uniformID = glGetUniformLocation(program, name); 
                glUniform2i(uniformID, value1, value2);
                return uniformID;
            }

            GLint setUniform3i(const char *name, GLint value1, GLint value2, GLint value3) {
                GLint uniformID = glGetUniformLocation(program, name); 
                glUniform3i(uniformID, value1, value2 , value3);
                return uniformID;
            }

            GLint setUniform4i(const char *name, GLint value1, GLint value2, GLint value3, GLint value4) {
                GLint uniformID = glGetUniformLocation(program, name); 
                glUniform4i(uniformID, value1, value2 , value3, value4);
                return uniformID;
            }

            GLint setUniform1fv(const char *name, const GLfloat *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniform1fv(name, count, v);
                return uniformID;
            }


            GLint setUniform2fv(const char *name, const GLfloat *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniform2fv(name, count, v);
                return uniformID;
            }

            GLint setUniform3fv(const char *name, const GLfloat *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniform3fv(name, count, v);
                return uniformID;
            }

            GLint setUniform4fv(const char *name, const GLfloat *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniform4fv(name, count, v);
                return uniformID;
            }

            GLint setUniform1iv(const char *name, const GLint *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniform1iv(name, count, v);
                return uniformID;
            }

            GLint setUniform2iv(const char *name, const GLint *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniform2iv(name, count, v);
                return uniformID;
            }


            GLint setUniform3iv(const char *name, const GLint *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniform3iv(name, count, v);
                return uniformID;
            }
            
            GLint setUniform4iv(const char *name, const GLint *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniform4iv(name, count, v);
                return uniformID;
            }

            GLint setUniform1uiv(const char *name, const GLuint *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniform1uiv(name, count, v);
                return uniformID;
            }

            GLint setUniform2uiv(const char *name, const GLuint *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniform2uiv(name, count, v);
                return uniformID;
            }

            GLint setUniform3uiv(const char *name, const GLuint *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniform3uiv(name, count, v);
                return uniformID;
            }

            GLint setUniform4uiv(const char *name, const GLuint *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniform4uiv(name, count, v);
                return uniformID;
            }

            GLint setUniformMatrix2fv(const char *name, GLfloat *v, GLuint count, GLboolean transpose,
                                            const GLfloat *values) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniformMatrix2fv(name, count, transpose, v);
                return uniformID;
            }

            GLint setUniformMatrix3fv(const char *name, GLfloat *v, GLuint count, GLboolean transpose,
                                            const GLfloat *values) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniformMatrix3fv(name, count, transpose, v);
                return uniformID;
            }

            GLint setUniformMatrix4fv(const char *name, GLfloat *v, GLuint count, GLboolean transpose,
                                            const GLfloat *values) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                    return uniformID;
                }
                glUniformMatrix4fv(name, count, transpose, v);
                return uniformID;
            }

            void drawElements() {
                    
                    for (GLuint i = 0; i < countAttributes; i++){                    
                        glBindBuffer(GL_ARRAY_BUFFER, vabID);
                        glEnableVertexAttribArray(attribute_coord3d);
                        glVertexAttribPointer(attribute_coord3d,
                                    attributeSize[i],
                                    GL_FLOAT,
                                    GL_FALSE,
                                    0,
                                    NULL
                                    );
                    }
                    
                    
                    
                        glBindBuffer(GL_ARRAY_BUFFER, nabID);
                        glEnableVertexAttribArray(attribute_normal3d);
                        glVertexAttribPointer(attribute_normal3d,
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
            }

            ~ShaderProxy() {
                delete [] attributeID;
                delete [] attributeSize;
                delete [] attributeBufferID;
                attributeBufferID = 0;
                uniformID = 0;
                attributeBufferID = 0;
            }
    };
}
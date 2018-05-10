#pragma once
#include <string>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glew.h"
#include <GL/freeglut.h>
#endif

#include "utils.hpp"
#include <map>

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

    	if (!link_ok) {
		    delete shadervs;
		    delete shaderfs;
            getErrorShaderInfoLog(program, 2);
		    return 0;
	    }

        delete shadervs;
        delete shaderfs;

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

    GLuint createTexture() {
        GLuint id;
        glGenTextures(1, &id);
        return id;
    }

    void setTextureImage(GLuint texID, const GLvoid *data, GLsizei width, GLsizei height, GLenum target=GL_TEXTURE_2D, 
                                GLint level=0, GLint internalFormat=GL_RGBA, GLint border=0, 
                                GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE) {
        glBindTexture(target, texID);
        glTexImage2D(target, level, internalFormat, width, height, border, format, type, data);
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
            GLuint *attributeBufferID;
            GLuint *attributeIndexCounter;
            
            GLuint *indexList;
            GLenum primitive;
            GLuint countIndex;
            GLuint indexBufferID;
            map<string, GLuint> id;
            GLuint idx;
            vector<GLuint> textureID;
            

        public:
            GLuint amor;
            ShaderProxy(GLuint program, GLuint countAttributes=1){
                this->program = program;
                this->countAttributes = countAttributes;
                this->attributeID = new GLint[countAttributes];
                this->attributeSize = new GLuint[countAttributes];
                this->attributeBufferID = new GLuint[countAttributes];
                this->attributeIndexCounter = new GLuint[countAttributes];
                this->idx = 0;
                this->indexList = 0;
            }

            GLint getAttribute(const char *name) {
                string key(name);
                GLuint i = 0;
                if (idx > countAttributes) {
                    throw string("Stack overflow. Maximum number of attributes achieved!");
                    return -1;
                }

                if (!id.count(key)){
                    id.insert(pair<string, GLuint>(key, idx));
                    i = idx;
                    idx++;
                } else {
                    i = id[key];
                }
                attributeID[i] = glGetAttribLocation(program, name);
                if (attributeID[i]<0) {
                    string msg("Attribute not found: ");
                    msg.append(name);
                    throw msg;
                }
                 
                return i;
            }


            void setAttribute(const char *name, GLfloat *values, GLuint countBytes, GLuint attributeSize=3,
                    GLenum mode = GL_STATIC_DRAW) {
                        GLint idx = getAttribute(name);
                        if (idx >= 0) {
                            setAttributeIdx(idx, values, countBytes, attributeSize, mode);
                        }
            }

            void setAttributeIdx(GLint idx, GLfloat *values, GLuint countBytes, GLuint attributeSize=3,
                    GLenum mode = GL_STATIC_DRAW) {                
                    this->attributeBufferID[idx] = createVertexBuffer();
                    this->attributeSize[idx] = attributeSize;
                    this->attributeIndexCounter[idx] = countBytes/sizeof(GLfloat);
                    setVertexDataBuffer(this->attributeBufferID[idx], values, countBytes, mode);
            }


            GLuint setTexture(GLvoid *values, GLsizei width, GLsizei height, GLuint attributeSize=2,
                                    GLenum target=GL_TEXTURE_2D,  GLint level=0, GLint internalFormat=GL_RGBA, 
                                    GLint border=0, GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE) {
                        GLuint idx = createTexture();
                        
                        setTextureImage(idx, values, width, height, target, 
                                            level, internalFormat, border, format, type);
            
                        textureID.push_back(idx);
                        return idx;
            }



            void setElementPrimitive(GLuint *indexList, GLuint countBytes,
                        GLenum primitive=GL_QUADS, GLenum mode=GL_STATIC_DRAW) {
                    this->indexList = indexList;
                    this->countIndex = countBytes/sizeof(indexList[0]);
                    this->primitive = primitive;
                    this->indexBufferID = createElementBuffer();
                    setElementDataBuffer(indexBufferID, indexList, countBytes,  mode);
            }

            GLint setUniform1f(const char *name, GLfloat value) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform1f(uniformID, value);
                return uniformID;
            }

            GLint setUniform2f(const char *name, GLfloat value1, GLfloat value2) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform2f(uniformID, value1, value2);
                return uniformID;
            }

            GLint setUniform3f(const char *name, GLfloat value1, GLfloat value2, GLfloat value3) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform3f(uniformID, value1, value2 , value3);
                return uniformID;
            }

            GLint setUniform4f(const char *name, GLfloat value1, GLfloat value2, GLfloat value3, GLfloat value4) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform4f(uniformID, value1, value2 , value3, value4);
                return uniformID;
            }

            GLint setUniform1i(const char *name, GLint value) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform1i(uniformID, value);
                return uniformID;
            }

            GLint setUniform2i(const char *name, GLint value1, GLint value2) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform2i(uniformID, value1, value2);
                return uniformID;
            }

            GLint setUniform3i(const char *name, GLint value1, GLint value2, GLint value3) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform3i(uniformID, value1, value2 , value3);
                return uniformID;
            }

            GLint setUniform4i(const char *name, GLint value1, GLint value2, GLint value3, GLint value4) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform4i(uniformID, value1, value2 , value3, value4);
                return uniformID;
            }

            GLint setUniform1fv(const char *name, const GLfloat *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform1fv(uniformID, count, v);
                return uniformID;
            }


            GLint setUniform2fv(const char *name, const GLfloat *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform2fv(uniformID, count, v);
                return uniformID;
            }

            GLint setUniform3fv(const char *name, const GLfloat *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform3fv(uniformID, count, v);
                return uniformID;
            }

            GLint setUniform4fv(const char *name, const GLfloat *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform4fv(uniformID, count, v);
                return uniformID;
            }

            GLint setUniform1iv(const char *name, const GLint *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform1iv(uniformID, count, v);
                return uniformID;
            }

            GLint setUniform2iv(const char *name, const GLint *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform2iv(uniformID, count, v);
                return uniformID;
            }


            GLint setUniform3iv(const char *name, const GLint *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform3iv(uniformID, count, v);
                return uniformID;
            }
            
            GLint setUniform4iv(const char *name, const GLint *v, GLuint count) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniform4iv(uniformID, count, v);
                return uniformID;
            }

            GLint setUniformMatrix2fv(const char *name, const GLfloat *values, GLuint count=1, GLboolean transpose=GL_FALSE) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniformMatrix2fv(uniformID, count, transpose, values);
                return uniformID;
            }

            GLint setUniformMatrix3fv(const char *name, const GLfloat *values, GLuint count=1, GLboolean transpose=GL_FALSE) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniformMatrix3fv(uniformID, count, transpose, values);
                return uniformID;
            }

            GLint setUniformMatrix4fv(const char *name, const GLfloat *values, GLuint count=1, GLboolean transpose=GL_FALSE) {
                GLint uniformID = glGetUniformLocation(program, name); 
                if (uniformID < 0) {
                        string msg("Uniform not found: ");
                        msg.append(name);
                        throw msg;
                        return -1;
                }
                glUniformMatrix4fv(uniformID, count, transpose, values);
                return uniformID;
            }

            void drawArrays(GLenum primitiveType, GLint attributeIndex = 0) {
                    for (GLint i = countAttributes-1; i >= 0; i--){                    
                        glBindBuffer(GL_ARRAY_BUFFER, attributeBufferID[i]);
                        glEnableVertexAttribArray(attributeID[i]);
                        glVertexAttribPointer(attributeID[i],
                                    attributeSize[i],
                                    GL_FLOAT,
                                    GL_FALSE,
                                    0,
                                    NULL
                                    );
                    }

                    glDrawArrays(
                        primitiveType,      // primitiva
                        0, //primeiro
                        this->attributeIndexCounter[attributeIndex]       // number of elements
                    );
            }

            void drawElements() {
                    for (GLuint i = 0; i < countAttributes; i++){                    
                        glBindBuffer(GL_ARRAY_BUFFER, attributeBufferID[i]);
                        glEnableVertexAttribArray(attributeID[i]);
                        glVertexAttribPointer(attributeID[i],
                                    attributeSize[i],
                                    GL_FLOAT,
                                    GL_FALSE,
                                    0,
                                    NULL
                                    );
                    }
                    
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

                    glDrawElements(
                        this->primitive,      // mode
                        this->countIndex,    // count
                        GL_UNSIGNED_INT,   // type
                        (void*)0           // element array buffer offset
                    );
            }

            void useProgram() {
                glUseProgram(program);
            }


            void close() {
                    for (GLuint i = 0; i < countAttributes; i++){            
                        glDisableVertexAttribArray(attributeID[i]);
                        glDeleteBuffers(1, &attributeBufferID[i]);
                    }
            }

            ~ShaderProxy() {
                glDeleteTextures(textureID.size(), &textureID[0]);
                delete [] attributeID;
                delete [] attributeSize;
                delete [] attributeBufferID;
                delete [] attributeIndexCounter;
                attributeBufferID = 0;
                attributeSize = 0;
                attributeID = 0;
                attributeIndexCounter = 0;
            }
    };
}
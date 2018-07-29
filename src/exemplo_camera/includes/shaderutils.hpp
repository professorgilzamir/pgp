/**
* Arquivo: shaderutils.hpp
* Este arquivo descreve a funções e classes do espaço de nome shaderutils.
*/
#pragma once

#include <string>

#ifdef __APPLE__ //se for um sistema macosx
#include <GLUT/glut.h> // faz este include
#else //caso contrario
#include "GL/glew.h" //inclui isso e
#include <GL/freeglut.h> //inclui isso
#endif //fim de papo

#include "utils.hpp" 
#include <map>

using namespace utils;
using namespace std;

/**
* Espaço de nome shaderutils: este espaço de nome contém todas as funções
* e classes que facilitam a troca de dados entre programa cliente e os
* shaders que rodam no hardware gráfico.
*/
namespace shaderutils {

    /**
    * Esta função lança uma exceção contendo informações do erro ocorrido, caso
    * algum erro de compilação ou ligação de shader tenha ocorrido.
    * @para GLuint target programa que está sendo compilado ou ligado.
    * @GLuint type tipo de shader (GL_VERTEX_SHADER ou GL_FRAGMENT_SHADER).
    */
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
    *  A funcao compila os shaders e retorna o identificador do programa 
    *  correspondente ou lanca uma exceccao cujo argumento eh uma string 
    *  indicando o erro que ocorreu.
    * @param vsname é o path do vertex shader a ser compilado e ligado.
    * @param fsname é o path do fragment shader a ser compilado e ligado.
    * @return GLuint o identificador do programa gerado.
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

    /**
    *
    *  Cria um GL_VERTEX_BUFFER.
    *  @return GLuint o identificador do buffer gerado.
    */
    GLuint createVertexBuffer() {
        GLuint bufferID;
        glGenBuffers(1, &bufferID);
        return bufferID;
    }

    /**
    * Cria um ELEMENT_ARRAY_BUFFER.
    * @return GLuint o identificador do buffer gerado.
    */
    GLuint createElementBuffer() {
        GLuint bufferID;
        glGenBuffers(1, &bufferID);
        return bufferID;
    }

    /**
    *
    * Cria uma textura.
    * @return o identificador da textura criada.
    */
    GLuint createTexture() {
        GLuint id;
        glGenTextures(1, &id);
        return id;
    }

    /**
    * Associa uma imagem à textura cujo identificador é <code>texID</code>.
    * Os dados da imagem são indicados pelo parâmetro <code>data</code>.
    * 
    * @param texID identificador da textura
    * @param data especifíca um ponteiro para os dados da imagem na memória.
    * @param width largura da imagem em pixels
    * @param height altura da imagem em pixels
    * @param target o objetivo das texturas. Valores possíveis são:
    * GL_TEXTURE_2D, GL_PROXY_TEXTURE_2D, GL_TEXTURE_1D_ARRAY, 
    * GL_PROXY_TEXTURE_1D_ARRAY, GL_TEXTURE_RECTANGLE, GL_PROXY_TEXTURE_RECTANGLE, 
    * GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 
    * GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    *  GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    * ou GL_PROXY_TEXTURE_CUBE_MAP.
    * @param level especifíca o nível de detalhe. Nível 0 é a imagem base. Nível n é a
    * n-ésima redução de mipmap. Se <code>target</code> é GL_TEXTURE_RECTANGLE 
    * ou GL_PROXY_TEXTURE_RECTANGLE, o nível deve ser 0.
    * @param internalFormat especifíca a quantidade de componentes de cor na textura.
    * Consulte documentação completa da função glTexImage2D para saber mais.
    * @param border Este valor deve ser 0 (zero).
    * @param format especifíca o formato dos dados dos pixels. Os seguintes valores
    * simbólicos são aceitos: GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, 
    * GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_BGR_INTEGER, GL_RGBA_INTEGER, 
    * GL_BGRA_INTEGER, GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL.
    * @param type Especifíca o tipo de dados dos dados dos pixels. Os seguintes valores
    * são aceitos: GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, 
    * GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, 
    * GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV,
    * GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, 
    * GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, 
    * GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, 
    * GL_UNSIGNED_INT_10_10_10_2, e GL_UNSIGNED_INT_2_10_10_10_REV.
    */
    void setTextureImage(GLuint texID, const GLvoid *data, GLsizei width, GLsizei height, GLenum target=GL_TEXTURE_2D, 
                                GLint level=0, GLint internalFormat=GL_RGBA, GLint border=0, 
                                GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE) {
        glBindTexture(target, texID);
        glTexImage2D(target, level, internalFormat, width, height, border, format, type, data);
    }


    /**
    * Permite fornecer os dados a serem armazenados em um vertex buffer com identificador
    * passado como argumento no parâmetro <code>bufferID</code>.
    * @param bufferID o identificador do vertex buffer onde os dados devem ser armazenados.
    * @param values um ponteiro para os dados a serem armazenados.
    * @param countBytes a quantidade de bytes a serem armazenados no buffer.
    * @param mode especifíca o padrão de uso dos dados a serem armazenados. As
    * constantes simbólicas devem ser GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,
    * GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ,
    * ou GL_DYNAMIC_COPY.
    */
    void setVertexDataBuffer(GLuint bufferID, GLfloat *values, GLuint countBytes, GLenum mode = GL_STATIC_DRAW) {
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ARRAY_BUFFER, countBytes, values, mode);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    
    /**
    * Permite fornecer os dados a serem armazenados em um element buffer com identificador
    * passado como argumento no parâmetro <code>bufferID</code>.
    * @param bufferID o identificador do element buffer onde os dados devem ser armazenados.
    * @param values um ponteiro para os dados a serem armazenados.
    * @param countBytes a quantidade de bytes a serem armazenados no buffer.
    * @param mode especifíca o padrão de uso dos dados a serem armazenados. As
    * constantes simbólicas devem ser GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,
    * GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ,
    * ou GL_DYNAMIC_COPY.
    */
    void setElementDataBuffer(GLuint bufferID, GLuint *values, GLuint countBytes, GLenum mode = GL_STATIC_DRAW) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, countBytes, values, mode);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }


    /**
    *
    * ShaderProxy é uma classe que facilita criar programas gráficos em C++
    * e OpenGL 2.1, reduzindo a quantidade de código necessário para conectar
    * dados entre o programa em C++ e o código em GLSL. Crie um objeto ShadeProxy
    * do seguinte modo:
    * <code>
    *  ShaderProxy *proxy = new ShaderProxy(id_programa, quantidadeDeAtributos);
    * </code>
    *  onde:
    *       id_programa é o identificador do programa gerado com a função genProgram e
    *       quantidadeDeAtributos é a quantidade de atributos que o programa GLSL possui.
    */
    class ShaderProxy {

        private:
            GLuint countAttributes; /** Quantidade de atributos */
            GLuint program; /** Identificador do programa GLSL */

            GLint *attributeID; /** Lista de identificadores dos atributos do programa GLSL associado ao ShaderProxy. */
            GLuint  *attributeSize; /** Cada atributo tem um tamanho. Por exemplo, o tamanho de um atributo do tipo vec3 é 3 e um do tipo float é 1. */
            GLuint *attributeBufferID; /** Uma lista de identificadores para cada buffer de atributo. */
            GLuint *attributeIndexCounter; /** Uma lista de índices para atributos de vértices. */
            
            GLuint *indexList; /** Ponteiro para uma lista de índices. */
            GLenum primitive; /** Tipo de primitiva a ser desennhada, por exemplo: GL_TRIANGLES, GL_QUADS, GL_LINES, GL_POINTS e outras. */
            GLuint countIndex; /** Tamanho da lista de índices. */
            GLuint indexBufferID; /** Identificador do buffer de índices. */
            map<string, GLuint> id; /** Associa o nome de um atributo ao identificador desse atributo. */
            GLuint idx; /** Variável que guardará o próximo índice. */
            vector<GLuint> textureID; /** Lista de identificadores de texturas. */
            

        public:
            /**
            * Construtor.
            * @param program Identificador do programa GLSL associado ao proxy.
            * @param countAttributes Quantidade de atributos no programa GLSL.
            */
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

            /**
            * Dado o nome do atributo <code>name</code>, retorna o identificador (ID) associado.
            * @param name Nome do atributo.
            * @return GLint identificador associado ao nome de atributo.
            */
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

            /**
            * Com essa função, pode-se definir um atributo e associar uma lista de valores
            * reais a serem armazenados em um buffer para este atributo.
            * @param name Nome do atributo, uma valor do tipo const char*.
            * @param values um ponteiro para a lista de numeros reais a serem armazenados.
            * @param countBytes a quantidade de bytes na lista de numeros reais.
            * @param attributeSize é o tamanho de cada atributo. Por exemplo, se o atributo
            * é do tipo vec3, o valor de attributeSize dever 3; se for vec4, o valor deve ser 4; e assim por diante.
            * especifíca o padrão de uso dos dados a serem armazenados. As
            * constantes simbólicas devem ser GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,
            * GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ,
            * ou GL_DYNAMIC_COPY.
            */
            void setAttribute(const char *name, GLfloat *values, GLuint countBytes, GLuint attributeSize=3,
                    GLenum mode = GL_STATIC_DRAW) {
                        GLint idx = getAttribute(name);
                        if (idx >= 0) {
                            setAttributeIdx(idx, values, countBytes, attributeSize, mode);
                        }
            }


            /**
            * Determina o valor de um atributo com base em seu índice interno.
            * @param idx índice interno do atributo.
            * @see Veja a documentação da função ShaderPoxy::setAttribute para o significado dos outros parâmetros.
            */
            void setAttributeIdx(GLint idx, GLfloat *values, GLuint countBytes, GLuint attributeSize=3,
                    GLenum mode = GL_STATIC_DRAW) {                
                    this->attributeBufferID[idx] = createVertexBuffer();
                    this->attributeSize[idx] = attributeSize;
                    this->attributeIndexCounter[idx] = countBytes/(attributeSize*sizeof(GLfloat));
                    setVertexDataBuffer(this->attributeBufferID[idx], values, countBytes, mode);
            }


            /**
            *   Gera uma textura de uma imagem cujo conteúdo é apontado pelo ponteiro <code>values</code>. 
            *   @see setTextureImage para saber o significado dos parâmetros.
            *   @return GLuint o identificador da textura gerada.
            */
            GLuint setTexture(GLvoid *values, GLsizei width, GLsizei height, GLuint attributeSize=2,
                                    GLenum target=GL_TEXTURE_2D,  GLint level=0, GLint internalFormat=GL_RGBA, 
                                    GLint border=0, GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE) {
                        GLuint idx = createTexture();
                        
                        setTextureImage(idx, values, width, height, target, 
                                            level, internalFormat, border, format, type);
            
                        textureID.push_back(idx);
                        return idx;
            }


            /**
            * Esta função define os elementos a serem renderizados. Elementos são as primitivas de desenho
            * da OpenGL, que podem ser pontos, linhas, triângulos ou faces quadráticas.
            * @param indeList lista de índices que formam os elementos.
            * @param countBytes quantidade de bytes na lista.
            * @param primitive o tipo de primitiva de cada elemento, por exemplo, GL_QUADS, 
            * GL_TRIANGLES, GL_LINES, dentre outros.
            * @param mode especifíca o padrão de uso dos dados a serem armazenados. As
            * constantes simbólicas devem ser GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,
            * GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ,
            * ou GL_DYNAMIC_COPY.
            */
            void setElementPrimitive(GLuint *indexList, GLuint countBytes,
                        GLenum primitive=GL_QUADS, GLenum mode=GL_STATIC_DRAW) {
                    this->indexList = indexList;
                    this->countIndex = countBytes/sizeof(indexList[0]);
                    this->primitive = primitive;
                    this->indexBufferID = createElementBuffer();
                    setElementDataBuffer(indexBufferID, indexList, countBytes,  mode);
            }

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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
            
            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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

            /**
            * Define o valor de um uniforme e retorna o seu ID.
            * @param name nome do uniforme.
            * @see Veja as funções glUniform* na documentação oficial da OpenGL e veja o que cada parâmetro significa. 
            */
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


            /**
            * Esta função funciona semelhante à função glDrawArrays da OpenGL, exceto que
            * encapsula muitas informações que já são obtidas do objeto ShaderProxy. Esta função,
            * dessa forma, desenha a primitiva indicada por <code>primitiveType</code> de acordo
            * com os dados informados no objeto ShaderProxy correspondente. Observe que, por padrão,
            * o atributo de onde os dados serão considerados é o de índice 0 (zero).
            * @param primitiveType Tipo de primitiva a ser desenhada, por exemplo, GL_TRIANGLES.
            * @param attributeIndex Índice do atributo de onde os dados de pontos devem ser extraídos.
            */
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

            /**
            * Desenha todos os elementos de acordo com a primitiva do objeto.
            */
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

            /**
            * Define que toda operação subsequente deve ser executada neste programa.
            */
            void useProgram() {
                glUseProgram(program);
            }



            /**
            * Libera buffers e dados alocados.
            */
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
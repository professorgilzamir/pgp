#pragma once

#include <string>

using namespace std;

namespace utils {

	/**
	*
	* Leitura de um arquivo com path name e armazenamento do conteudo em um array de strings
	*
	*/
	string* readfile(const char *name){
		ifstream source(name);
		string line;
		string *content = new string();
		if (source.is_open()){
			while (!source.eof()) {
				getline(source, line);
				content->append(line);
				content->append("\n");
			}
			source.close();
		}
		return content;
	}

	void getNormalFromVertex(GLuint vertexIndex, GLuint *faceIndex, GLuint *normalsIndex, GLfloat *normals,
								GLuint numberOfFaces, GLuint numVerticesInFace,  GLfloat out[3]){
		out[0] = 0.0f;
		out[1] = 0.0f;
		out[2] = 0.0f;


		for (GLuint i = 0; i < numberOfFaces; i++) {
			GLuint pos = numVerticesInFace * i;
			for (GLuint j = 0; j < numVerticesInFace; j++) {
				GLuint idx = faceIndex[pos+j];
				if (idx == vertexIndex) {
					GLuint nidx = i * 3;
					out[0] += normals[nidx];
					out[1] += normals[nidx+1];
					out[2] += normals[nidx+2];
				}
			}
		}
	}


	void getNormals(GLuint numberOfVertices, GLuint *faceIndex, GLuint *normalsIndex, GLfloat *normals,
								GLuint numberOfFaces, GLuint numVerticesInFace, GLfloat *out) {
		for (GLuint i = 0; i < numberOfVertices; i++) {
			GLfloat n[3];
			getNormalFromVertex(i, faceIndex, normalsIndex, normals, numberOfFaces, numVerticesInFace, n);
			int idx = i * 3;
			out[idx] = n[0];
			out[idx+1] = n[1];
			out[idx+2] = n[2];
		}
	}
}
#version 120
attribute vec2 vertice;
uniform mat2 transformacao;

void  main(void) {
	vec2 verticeTransformado = transformacao * vertice;
	gl_Position = vec4(verticeTransformado, 0.0, 1.0);
}

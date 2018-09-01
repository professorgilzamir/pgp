#version 120
attribute vec3 posicao;
uniform vec4 cor;
uniform mat4 transformacoes;
varying vec4 corFinal;
uniform float pointSize;

void  main(void) {
	corFinal = cor;
	vec4 pos = transformacoes * vec4(posicao, 1);
	gl_PointSize = pointSize;
	gl_Position =  pos;
}

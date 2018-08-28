#version 120
attribute vec2 posicao;
uniform vec4 cor;
uniform mat2 transformacoes;
varying vec4 corFinal;
uniform float pointSize;
uniform float lineSize;

void  main(void) {
	corFinal = cor;
	vec2 pos = transformacoes * posicao;
	gl_PointSize = pointSize;
	gl_Position =  vec4(pos, 0.0, 1.0);
}

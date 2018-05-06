#version 120
attribute vec2 posicao;
uniform vec4 cor;

varying vec4 corFinal;

void  main(void) {
	corFinal = cor;
	gl_Position =  vec4(posicao, 0.0, 1.0);
}

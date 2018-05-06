#version 120
attribute vec2 posicao;

void  main(void) {
	gl_PointSize = 20;
	gl_Position =  vec4(posicao, 0.0, 1.0);
}

#version 120
attribute vec3 posicao;
uniform vec4 cor;
uniform mat4 projecao;
uniform mat4 modelo;
uniform mat4 camera;
varying vec4 corFinal;
uniform float pointSize;

void  main(void) {
	corFinal = cor;
	vec4 pos = projecao * camera * modelo * vec4(posicao, 1);
	gl_PointSize = pointSize;
	gl_Position =  pos;
}

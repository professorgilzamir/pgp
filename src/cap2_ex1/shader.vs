#version 120
attribute vec2 coord2d;
uniform mat2 transformacao;

void  main(void) {
	vec2 coordenadas = transformacao * coord2d;
	gl_Position = vec4(coordenadas, 0.0, 1.0);
}

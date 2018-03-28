#version 120
attribute vec2 coord2d;
uniform vec3 color;
varying vec3 vColor;
void  main(void) {
	vColor = color;
	gl_Position = vec4(coord2d, 0.0, 1.0);
}

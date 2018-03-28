#version 120
attribute vec2 coord2d;
attribute vec4 colors;
varying vec4 fColor; 
void  main(void) {
	fColor = colors;
	gl_Position = vec4(coord2d, 0.0, 1.0);
}

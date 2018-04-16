#version 120
attribute vec3 coord3d;
attribute vec3 color3d;
uniform mat4 matrix;
varying vec4 fcolor;
void  main(void) {
	fcolor = vec4(color3d, 1.0);
	gl_Position =  matrix * vec4(coord3d, 1.0);
}

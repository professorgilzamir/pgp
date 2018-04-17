#version 120
attribute vec3 coord3d;
uniform mat4 matrix;
void  main(void) {
	gl_Position =  matrix * vec4(coord3d, 1.0);
}

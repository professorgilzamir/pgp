#version 120
attribute vec3 coord3d;
uniform mat4 model;
void  main(void) {
	gl_Position =  model * vec4(coord3d, 1.0);
}

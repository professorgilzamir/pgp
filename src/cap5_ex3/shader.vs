#version 120
attribute vec3 coord3d;
attribute vec3 color3d;
attribute vec2 texcoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
varying vec4 fcolor;
varying vec2 ftexcoord;
void  main(void) {
	ftexcoord = texcoord;
	fcolor = vec4(color3d, 1.0);
	gl_Position =  projection * view * model * vec4(coord3d, 1.0);
}

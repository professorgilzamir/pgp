#version 120
attribute vec3 coord3d;
attribute vec4 colors;
uniform mat4 model;
uniform mat4 world;
uniform mat4 camera;
uniform mat4 projection;

varying vec4 fColor; 

void  main(void) {
	fColor = colors;
	gl_PointSize = 10;
	gl_Position = projection * camera * world * model * vec4(coord3d, 1.0);
}

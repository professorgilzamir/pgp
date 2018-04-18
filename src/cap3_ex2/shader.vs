#version 120
attribute vec3 coord3d;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
varying vec4 finalColor;
uniform vec4 materialColor;


void  main(void) {
	vec4 ambColor = vec4(0.2, 0.2, 0.2, 1.0);



	finalColor = ambColor * materialColor;
	gl_Position =  projection * view * model * vec4(coord3d, 1.0);
}

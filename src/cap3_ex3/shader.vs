#version 120
attribute vec3 coord3d;
attribute vec3 normal3d;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
varying vec4 finalColor;
uniform vec4 materialColor;


void  main(void) {

	vec4 posFinal = model * vec4(coord3d, 1.0);

	vec3 lightPos = vec3(0, 10, -20);
	vec4 lightColor = vec4(0.8, 0.8, 0.8, 1.0);
	vec3 L = normalize(lightPos-vec3(posFinal));
	vec3 N = normalize(normal3d);

	finalColor = vec4(0.4, 0.4, 0.4, 1.0) * materialColor + materialColor * lightColor * max(0, dot(N,L));

	gl_Position =  projection * posFinal;
}

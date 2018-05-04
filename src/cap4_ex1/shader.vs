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

	vec3 lightPos = vec3(0, 10, -10);
	vec4 lightColor = vec4(0.8, 0.8, 0.8, 1.0);

	vec4 ambienteColor = vec4(0.4, 0.4, 0.4, 1.0);

	vec4 shinenessColor = vec4(0.8, 0.8, 0.8, 1.0);

	float shineness = 1.0;

	vec3 L = normalize(lightPos-vec3(posFinal));
	vec3 N = normalize(normal3d);

	vec3 R = normalize(2 * dot(N, L) - L);

	finalColor = ambienteColor * materialColor //iluminacao ambiente
							+ materialColor * lightColor * max(0, dot(N,L)) //reflexao difusa
							+ shinenessColor * lightColor * pow(max(0, dot(N, R)),shineness); //reflexao especular

	gl_Position =  projection * posFinal;
}

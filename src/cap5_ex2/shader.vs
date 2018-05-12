#version 120
attribute vec3 coord3d; //coordenadas do vertice
attribute vec2 texcoord; //coordenadas de textura mapeadas para o vertice
attribute vec3 normal;

uniform mat3 normalMatrix;
uniform mat4 model; //matriz de transformacao do modelo
uniform mat4 view; //matriz de posicao e orientacao da camera
uniform mat4 projection; //matriz de projecao
varying vec2 ftexcoord; //coordenadas de textura interpolada para cada pixel
varying vec4 fcolor;

void  main(void) {

	vec4 material = vec4(0.8, 0.8, 0.8, 1.0);
	vec3 posicaoDaLuz = vec3(30, 30, 30);


	vec4 posicaoFinal = model * vec4(coord3d,1.0);
	vec3 normalFinal = normalize(normalMatrix * normal);
	vec3 direcaoLuz = normalize(posicaoDaLuz - vec3(posicaoFinal));

	fcolor = material * max(0, dot(normalFinal, direcaoLuz));


	ftexcoord = texcoord;
	gl_Position =  projection * view * posicaoFinal;
}

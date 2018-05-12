#version 120
attribute vec3 coord3d; //coordenadas do vertice
attribute vec2 texcoord; //coordenadas de textura mapeadas para o vertice
uniform mat4 model; //matriz de transformacao do modelo
uniform mat4 view; //matriz de posicao e orientacao da camera
uniform mat4 projection; //matriz de projecao
varying vec2 ftexcoord; //coordenadas de textura interpolada para cada pixel
void  main(void) {
	ftexcoord = texcoord;
	gl_Position =  projection * view * model * vec4(coord3d, 1.0);
}

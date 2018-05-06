#version 120

attribute vec2 posicao;

void main() 
{
	gl_Position = vec4(posicao, 0, 1);
}
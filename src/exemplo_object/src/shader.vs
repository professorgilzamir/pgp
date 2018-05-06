#version 120

attribute vec2 posicao;
uniform mat4 transform;

void main() 
{
	gl_Position = transform * vec4(posicao, 0, 1);
}
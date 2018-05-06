#version 120

attribute vec2 posicao;
uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
	gl_Position = projection * view * transform * vec4(posicao, -1, 1);
}
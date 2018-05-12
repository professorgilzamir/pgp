#version 120

varying vec4 fcolor;
varying vec2 ftexcoord;
uniform sampler2D tex;
uniform sampler2D other;

void main(void){
	gl_FragColor = texture2D(tex, ftexcoord)  *  texture2D(other, ftexcoord)  + 0.2 * fcolor;	
}

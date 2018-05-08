#version 120

varying vec4 fcolor;
varying vec2 ftexcoord;
uniform sampler2D tex;

void main(void){
	gl_FragColor = texture2D(tex, ftexcoord) + 0.0001 * fcolor;	
}

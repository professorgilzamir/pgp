#version 120

varying vec2 ftexcoord;
uniform sampler2D tex;

void main(void){
	gl_FragColor = texture2D(tex, ftexcoord);	
}

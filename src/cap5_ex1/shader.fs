#version 120

varying vec2 ftexcoord;
uniform sampler2D tex;
uniform sampler2D tex2;

void main(void){
	gl_FragColor = 0.9 * texture2D(tex, ftexcoord) + 0.3 * texture2D(tex2, ftexcoord);	
}

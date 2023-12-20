#version 450
out vec4 FragColor;
uniform sampler2D _ColorBuffer;
in vec2 UV;
void main(){
	FragColor = texture(_ColorBuffer,UV);
}
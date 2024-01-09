#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
}fs_in;

uniform sampler2D _MainTex;

void main(){
	vec3 albedo = texture(_MainTex,fs_in.UV).rgb;
	FragColor = vec4(albedo,1.0);
}
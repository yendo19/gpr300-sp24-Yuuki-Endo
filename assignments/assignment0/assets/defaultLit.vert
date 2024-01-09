#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
}vs_out;

uniform mat4 _Model;
uniform mat4 _ViewProjection;

void main(){
	vs_out.UV = vUV;
	vs_out.WorldPosition = (_Model * vec4(vPos,1.0)).xyz;
	vs_out.WorldNormal = inverse(transpose(mat3(_Model))) * vNormal;
	gl_Position = _ViewProjection * vec4(vs_out.WorldPosition,1.0);
}
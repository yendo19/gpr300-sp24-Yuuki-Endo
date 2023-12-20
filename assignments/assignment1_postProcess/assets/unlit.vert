#version 450

layout(std140, binding = 0) uniform PerFrameData
{
	uniform mat4 _ViewProjection;
};

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

uniform mat4 _Model;

void main(){
	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}
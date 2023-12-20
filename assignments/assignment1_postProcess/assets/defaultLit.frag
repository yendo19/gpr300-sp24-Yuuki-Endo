#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPosition;
	vec3 WorldNormal;
}fs_in;

uniform sampler2D _Texture;

uniform struct Light{
	vec3 position;
	vec3 color;
};
#define MAX_LIGHTS 4
uniform Light _Lights[MAX_LIGHTS];

struct Material{
	float ambientK;
	float diffuseK;
	float specularK;
	float shininess;
};
uniform Material _Material;
uniform vec3 _EyePos;
uniform int _NumLights = MAX_LIGHTS; //Active # of lights to calculate

vec3 calcLight(Light light, vec3 normal){
	float ambient = _Material.ambientK;

	//Diffuse
	vec3 toLight = normalize(light.position.xyz - fs_in.WorldPosition.xyz);
	float diffuse = max(dot(toLight,normal),0.0) * _Material.diffuseK;

	//Specular
	vec3 toEye = normalize(_EyePos - fs_in.WorldPosition.xyz);
	vec3 halfVector = normalize(toLight + toEye);
	float specular = dot(halfVector,normal);
	specular = pow(max(specular,0.0),_Material.shininess) * _Material.specularK;

	return (ambient + diffuse + specular) * light.color;
}

void main(){
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 albedo = texture(_Texture,fs_in.UV).rgb;
	vec3 totalLight = vec3(0);
	for(int i = 0; i < _NumLights; i++){
		totalLight += calcLight(_Lights[i],normal);
	}
	FragColor = vec4(albedo * totalLight,1.0);
	
}
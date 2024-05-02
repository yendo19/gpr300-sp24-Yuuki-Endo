#version 450
out vec4 FragColor;
in vec2 UV;
uniform sampler2D _ColorBuffer;

uniform float _Red = 0;
uniform float _Blue = 0;

void main(){
//https://gamedev.stackexchange.com/questions/58408/how-would-you-implement-chromatic-aberration
//https://www.youtube.com/watch?v=3CsNRBme6nU
vec3 color = texture(_ColorBuffer,UV).rgb;

//vec2 texelSize = 1.0 / textureSize(_ColorBuffer,0).xy;
//vec3 totalColor = vec3(0);
//for(int y = -2; y <= 2; y++){
//   for(int x = -2; x <= 2; x++){
//      vec2 offset = vec2(x,y) * texelSize;
//      totalColor += texture(_ColorBuffer,UV + offset).rgb;
//   }
//}
//totalColor/=(5 * 5);
//FragColor = vec4(totalColor,1.0);
	float red = texture(_ColorBuffer,UV - _Red).g;
	float blue = texture(_ColorBuffer,UV - _Blue).b;

	 color.r= red;
	 color.b = blue;

FragColor = vec4(color, 1.0);
}

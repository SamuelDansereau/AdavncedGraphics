#version 450                          
out vec4 FragColor;

in vec3 Normal;
in vec2 UV;
in mat3 TBN;

uniform sampler2D activeTexture;
uniform sampler2D normalMap;

uniform float _Time;	
uniform float _Speed;
float t = _Time / _Speed;

void main(){    
	vec3 normal = texture(normalMap,UV).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	vec3 nWorld = normalize(TBN * normal);
	FragColor = vec4(nWorld, 1);
}

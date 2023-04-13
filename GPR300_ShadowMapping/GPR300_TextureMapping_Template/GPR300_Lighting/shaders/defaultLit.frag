#version 450                          
out vec4 FragColor;

in vec3 Normal;
in vec2 UV;
in mat3 TBN;
in vec4 lightSpacePos;

uniform sampler2D activeTexture;
uniform sampler2D normalMap;
uniform float normalIntensity;
uniform sampler2D _ShadowMap;
uniform float _minBias, _maxBias;

in struct Vertex{
    vec3 WorldNormal;
    vec3 WorldPosition;
}v_out;

struct Material
{
	vec3 Color;
	float ambientK, diffuseK, specularK, shininess;
};

struct DirLight{
    vec3 dir;
    vec3 color;
    float intensity;
};

uniform Material _Material;
uniform DirLight _dLight;
uniform vec3 camPos;

float calcShadow(sampler2D sm, vec4 lsp, vec3 norm, vec3 toLight)
{
    vec3 samplecoord = lsp.xyz / lsp.w;
    samplecoord = samplecoord * 0.5 + 0.5;
    float myDepth = samplecoord.z;

    float bias = max(_maxBias * (1.0 - dot(normalize(norm),toLight)),_minBias);
    
    float allShadows = 0;
    vec2 texelOffset = 1.0 / textureSize(sm, 0);
    for(int y = -1; y <=1; y++){
	    for(int x = -1; x <=1; x++){
		    float depth = texture(sm, samplecoord.xy + vec2(y, x) * texelOffset).r;
            allShadows+=step(depth + bias, myDepth);
        }
    }
    allShadows/=9.0;

    return allShadows;
}


void main(){   

    //Andrew let me borrow his directional light since mine wasn't working well and we thought it might be why the shadows broke 
    vec3 normal = normalize(v_out.WorldNormal);

    vec3 ambient = _dLight.color * _Material.ambientK * _dLight.intensity/ 100 ;

    vec3 diffuse = _Material.diffuseK * max(dot(normalize(-_dLight.dir), normal), 0) * _dLight.color * _dLight.intensity / 100;

    vec3 viewer = normalize(camPos - v_out.WorldPosition);
    vec3 H = normalize(viewer + normalize(-_dLight.dir));
    vec3 specular = _Material.specularK * pow(max(dot(normal, H), 0), _Material.shininess) * _dLight.color * _dLight.intensity / 100;

    vec3 lightColor = diffuse + specular;

    float shadow = calcShadow(_ShadowMap, lightSpacePos, normal, -_dLight.dir);
    lightColor *= (1 - shadow);
    lightColor + ambient;

    FragColor = vec4(lightColor * _Material.Color, 1) * texture(activeTexture, UV);
}

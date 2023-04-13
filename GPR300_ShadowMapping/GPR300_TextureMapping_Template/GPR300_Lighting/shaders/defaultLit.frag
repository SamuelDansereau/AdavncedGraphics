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


struct PointLight{
    vec3 pos;
    vec3 color;
    float intensity;
    float radius;
};

uniform Material _Material;
uniform PointLight _pLight;
uniform DirLight _dLight;
uniform vec3 camPos;

vec3 calcBlinnPhong(float lightIntensity, vec3 dir, vec3 normal)
{
    //Diffuse
    vec3 norm = normalize(v_out.WorldNormal * normal);
    dir = normalize(-dir);
    vec3 diffuse = _Material.diffuseK * max(dot(dir, norm),0.0f) * _Material.Color * lightIntensity;

    //Specular
    vec3 dirToView = normalize(camPos - v_out.WorldPosition);
    vec3 specular = _Material.specularK * pow(max(dot(norm, normalize(dirToView)),0),_Material.shininess) * _Material.Color * lightIntensity;

    return diffuse + specular;
}

vec3 calcDirectionalLight(DirLight dl, vec3 norm, float shadow)
{
    vec3 blinnPhong = calcBlinnPhong(dl.intensity, dl.dir, norm) * (1.0 * shadow);
    //Ambient
    vec3 ambient = _Material.ambientK * _Material.Color * dl.intensity; 
    blinnPhong += ambient;
    vec3 light = blinnPhong * dl.color;
    return light;
}

vec3 calcPointLight(PointLight pl, vec3 normal)
{
    vec3 dir = pl.pos - v_out.WorldNormal;
    dir = normalize(dir);
    float dist = length(dir);


    vec3 blinnPhong = calcBlinnPhong(pl.intensity, dir, normal);

    vec3 light = blinnPhong * pl.color;
    float la = pow((pl.radius/max(pl.radius, dist)),2);

    return light * la;
}

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

uniform float _Time;	
uniform float _Speed;
float t = _Time / _Speed;

void main(){    
	vec3 normal = texture(normalMap,UV).rgb;
	normal = normalize(normal * 2.0 - 1.0);
    normal *= vec3(normalIntensity, normalIntensity, 1);
    float shadow = calcShadow(_ShadowMap, lightSpacePos, normal, _dLight.dir);
    vec3 lightColor = calcDirectionalLight(_dLight, normal, shadow);    //calcPointLight(_pLight, normal);
	FragColor = vec4(lightColor * _Material.Color, 1) * texture(activeTexture, UV);
} 

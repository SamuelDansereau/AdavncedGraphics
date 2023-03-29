#version 450                          
out vec4 FragColor;

in vec3 Normal;
in vec2 UV;
in mat3 TBN;

uniform sampler2D activeTexture;
uniform sampler2D normalMap;
uniform float normalIntensity;

in struct Vertex{
    vec3 WorldNormal;
    vec3 WorldPosition;
}v_out;

struct Material
{
	vec3 Color;
	float ambientK, diffuseK, specularK, shininess;
};

struct PointLight{
    vec3 pos;
    vec3 color;
    float intensity;
    float radius;
};

uniform Material _Material;
uniform PointLight _pLight;
uniform vec3 camPos;

vec3 calcBlinnPhong(float lightIntensity, vec3 dir, vec3 normal)
{

    //Ambient
    vec3 ambient = _Material.ambientK * _Material.Color * lightIntensity ; 

    //Diffuse
    vec3 norm = normalize(v_out.WorldNormal * normal);
    dir = normalize(-dir);
    vec3 diffuse = _Material.diffuseK * max(dot(dir, norm),0.0f) * _Material.Color * lightIntensity;

    //Specular
    vec3 dirToView = normalize(camPos - v_out.WorldPosition);
    vec3 specular = _Material.specularK * pow(max(dot(norm, normalize(dirToView)),0),_Material.shininess) * _Material.Color * lightIntensity;


    return ambient + diffuse + specular;
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

uniform float _Time;	
uniform float _Speed;
float t = _Time / _Speed;

void main(){    
	vec3 normal = texture(normalMap,UV).rgb;
	normal = normalize(normal * 2.0 - 1.0);
    normal *= vec3(normalIntensity, normalIntensity, 1);
    vec3 lightColor = calcPointLight(_pLight, normal);
	FragColor = vec4(lightColor * _Material.Color, 1) * texture(activeTexture, UV);
}

#version 450                          
out vec4 FragColor;

struct Vertex{
    vec3 WorldNormal;
    vec3 WorldPos;
};


struct Material
{
	vec3 Color;
	float ambientK, diffuseK, specularK, shininess;
};

struct Light{
    vec3 pos;
    vec3 color;
    float intensity;
};

struct DirLight{
    vec3 dir;
    vec3 color;
    float intensity;
};

#define MAX_LIGHTS  8
uniform Material _Material;
uniform Light _Lights[MAX_LIGHTS];
uniform vec3 camPos;
uniform vec3 _Color;

vec3 calcBlinnPhong(Material mat, DirLight light)
{
    vec3 ambient = mat.Color * light.color * mat.ambientK; 

    vec3 diffuse = mat.diffuseK * (light.dir) * light.intensity;

    return ambient;
}


void main(){         
    Vertex vert;
    FragColor = vec4(abs(vert.WorldNormal),1.0f);
}

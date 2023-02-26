#version 450                          
out vec4 FragColor;

out struct Vertex{
    vec3 WorldNormal;
    vec3 WorldPos
}v_out;


struct Material
{
	glm::vec3 Color;
	float ambientK, diffuseK, specularK, shininess;
};

struct Light{
    vec3 pos;
    vec3 color;
    float intensity;
};

#define MAX_LIGHTS  8
uniform Material _Material
uniform Light _Lights[MAX_LIGHTS];
uniform vec3 camPos;
uniform vec3 _Color;

vec3 calcBlinnPhong(Material mat, Light light)
{
    vec3 ambient = mat.Color * light.Color * mat.ambientK; 
}


void main(){         
    FragColor = vec4(abs(Normal),1.0f);
}

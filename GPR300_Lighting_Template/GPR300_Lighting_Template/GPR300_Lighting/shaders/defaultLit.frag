#version 450                          
out vec4 FragColor;

in struct Vertex{
    vec3 WorldNormal;
    vec3 WorldPosition;
}v_out;

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

struct PointLight{
    vec3 pos;
    vec3 color;
    float intensity;
    float linearAttenuation;

};

struct SpotLight{
    vec3 pos;
    vec3 dir;
    vec3 color;
    float intensity;
    float linearAttenuation;
    float minAnlge, maxAnlge;

};

#define MAX_LIGHTS  8
uniform Material _Material;
uniform Light _Lights[MAX_LIGHTS];
uniform DirLight _dLight;
uniform vec3 camPos;
uniform vec3 _Color;

float calcBlinnPhong(float lightIntensity, vec3 dir)
{

    //Ambient
    float ambient = _Material.ambientK * lightIntensity; 

    //Diffuse
    vec3 norm = normalize(v_out.WorldNormal);
    dir = normalize(-dir);
    float diffuse = _Material.diffuseK * max(dot(dir, norm),0.0f) * lightIntensity;

    //Specular
    vec3 dirToView = normalize(camPos - v_out.WorldPosition);
    float specular = _Material.specularK * pow(max(dot(reflect(dir,v_out.WorldNormal), dirToView), 0.0f),_Material.shininess) * lightIntensity;


    return ambient + diffuse + specular;
}

vec3 calcDirectionalLight(DirLight dl)
{
    float blinnPhong = calcBlinnPhong(dl.intensity, dl.dir);
    vec3 light = blinnPhong * dl.color;
    return light;
}


void main()
{         
    vec3 normal = normalize(v_out.WorldNormal);

    vec3 matCol =  _Material.Color;

    vec3 lightColor = calcDirectionalLight(_dLight);

    vec3 finalColor = matCol * lightColor;

    FragColor = vec4((finalColor),1.0f);
}

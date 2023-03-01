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
    float radius;
};

struct SpotLight{
    vec3 pos;
    vec3 dir;
    vec3 color;
    float intensity;
    float linearAttenuation;
    float minAnlge, maxAnlge;

};

#define MAX_LIGHTS 2
uniform Material _Material;
uniform PointLight _pLights[MAX_LIGHTS];
uniform DirLight _dLight;
uniform SpotLight _sLight;
uniform vec3 camPos;
uniform vec3 _Color;

vec3 calcBlinnPhong(float lightIntensity, vec3 dir)
{

    //Ambient
    vec3 ambient = _Material.ambientK * _Material.Color * lightIntensity ; 

    //Diffuse
    vec3 norm = normalize(v_out.WorldNormal);
    dir = normalize(-dir);
    vec3 diffuse = _Material.diffuseK * max(dot(dir, norm),0.0f) * _Material.Color * lightIntensity;

    //Specular
    vec3 dirToView = normalize(camPos - v_out.WorldPosition);
    vec3 specular = _Material.specularK * pow(max(dot(norm, normalize(dirToView)),0),_Material.shininess) * _Material.Color * lightIntensity;


    return ambient + diffuse + specular;
}

vec3 calcDirectionalLight(DirLight dl)
{
    vec3 blinnPhong = calcBlinnPhong(dl.intensity, dl.dir);
    vec3 light = blinnPhong * dl.color;
    return light;
}

vec3 calcPointLight(PointLight pl)
{
    vec3 dir = pl.pos - v_out.WorldNormal;
    dir = normalize(dir);
    float dist = length(dir);


    vec3 blinnPhong = calcBlinnPhong(pl.intensity, dir);

    vec3 light = blinnPhong * pl.color;
    float la = pow((pl.radius/max(pl.radius, dist)),2);

    return light * la;
}

vec3 calcSpotLight(SpotLight sl)
{
    vec3 dir = sl.pos - v_out.WorldNormal;
    dir = normalize(dir);
    float theta = dot(-dir, normalize(sl.dir));

    float angularAttenuation = pow(clamp((theta - sl.maxAnlge) / (sl.minAnlge - sl.maxAnlge), 0.0, 1.0), sl.linearAttenuation);

    vec3 light = calcBlinnPhong(sl.intensity, dir) * sl.color;

    return light * angularAttenuation;
}


void main()
{         
    vec3 normal = normalize(v_out.WorldNormal);


    vec3 lightColor = calcDirectionalLight(_dLight);
    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        lightColor += calcPointLight(_pLights[i]);
    }

    lightColor += calcSpotLight(_sLight);

    vec3 finalColor = _Material.Color * lightColor;

    FragColor = vec4((finalColor),1.0f);
}

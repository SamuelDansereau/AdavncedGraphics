#version 450                          
layout (location = 0) in vec3 vPos;  
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec3 vTangent;


uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;
uniform mat4 _LightViewProj;

out struct Vertex{
    vec3 WorldNormal;
    vec3 WorldPosition;
}v_out;

out vec3 Normal;
out vec2 UV;
out vec3 Tangent;
out mat3 TBN;
out vec4 lightSpacePos;




void main(){   
    v_out.WorldPosition = vec3(_Model * vec4(vPos,1));

    Normal = vNormal;
    UV = vUV;
    Tangent = normalize(vec3(_Model * vec4(vTangent,0)));
    vec3 BiTangent = cross(Normal,Tangent);
    TBN = mat3(
    Tangent.x, BiTangent.x, Normal.x,
    Tangent.y, BiTangent.y, Normal.y,
    Tangent.z, BiTangent.z, Normal.z
    );

    TBN *= transpose(inverse(mat3(_Model)));
    Normal *= TBN;
    v_out.WorldNormal = Normal;
    lightSpacePos = _LightViewProj * _Model * vec4(vPos, 1);
    gl_Position = _Projection * _View * _Model * vec4(vPos,1);
}

#version 450                          
layout (location = 0) in vec2 vPos;  
layout (location = 1) in vec3 vColor;

//out vec3 Normal;
//out vec2 UV;

out VS_OUT
{
    vec3 color;
}vs_out;

void main(){    
    //Normal = vNormal;
    //UV = vUV;
    gl_Position = vec4(vPos.x, vPos.y, 0.0, 1.0);
    vs_out.color = vColor;
}

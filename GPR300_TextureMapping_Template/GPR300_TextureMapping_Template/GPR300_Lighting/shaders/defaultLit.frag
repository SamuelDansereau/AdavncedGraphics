#version 450                          
out vec4 FragColor;

in vec3 Normal;
in vec2 UV;

uniform sampler2D activeTexture;
uniform float _Time;	
uniform float _Speed;
float t = _Time / _Speed;

void main(){         
    FragColor = vec4(1,1,1,1.0f) * texture(activeTexture, UV + t);
}

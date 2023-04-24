#version 450                          
out vec4 FragColor;

in vec3 fColor;

//in vec3 Normal;
//in vec2 UV;

void main(){         
    FragColor = vec4(fColor, 1.0);
}

// Following a tutorial from learnopenGL just to get a foundation on this down https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

#version 450         
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;
out vec3 fColor;

vec4 pos = gl_in[0].gl_Position;

in VS_OUT
{
    vec3 color;
} gs_in[];

void main() {    
    fColor = gs_in[0].color;
    gl_Position = pos + vec4(-0.15, -0.15, 0.0, 0.0); 
    EmitVertex();

    gl_Position = pos + vec4(0.15, -0.15, 0.0, 0.0); 
    EmitVertex();

    gl_Position = pos + vec4(-0.15, 0.15, 0.0, 0.0); 
    EmitVertex();

    gl_Position = pos + vec4(0.15, 0.15, 0.0, 0.0); 
    EmitVertex();

    gl_Position = pos + vec4(0.0, 0.3, 0.0, 0.0); 
    EmitVertex();

    EndPrimitive();
}  
// Following a tutorial from learnopenGL just to get a foundation on this down https://learnopengl.com/Advanced-OpenGL/Geometry-Shader

#version 450         
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;
out vec3 fColor;
uniform int _activeGeom;

vec4 pos = gl_in[0].gl_Position;

in VS_OUT
{
    vec3 color;
} gs_in[];

void main() {   
    if(_activeGeom == 0)
    {
        fColor = gs_in[0].color;
        gl_Position = pos + vec4(-0.3, -0.3, 0.0, 0.0); // bottom left
        EmitVertex();

        gl_Position = pos + vec4(0.3, -0.3, 0.0, 0.0); // bottom right
        EmitVertex();

        gl_Position = pos + vec4(-0.3, 0.3, 0.0, 0.0); //top left
        EmitVertex();

        gl_Position = pos + vec4(0.3, 0.3, 0.0, 0.0); // top right 
        EmitVertex();

        gl_Position = pos + vec4(0.0, 0.6, 0.0, 0.0); // top 
        EmitVertex();

        EndPrimitive();
    }
    else if(_activeGeom == 3)
    {
        fColor = gs_in[0].color;
        gl_Position = pos + vec4(-0.5, -0.8, 0.0, 0.0); // bottom left
        EmitVertex();

        gl_Position = pos + vec4(0.2, -0.1, 0.0, 0.0); // bottom right
        EmitVertex();

        gl_Position = pos + vec4(-0.3, 0.4, 0.0, 0.0); //top left
        EmitVertex();

        gl_Position = pos + vec4(0.2, 0.7, 0.0, 0.0); // top right 
        EmitVertex();

        gl_Position = pos + vec4(0.6, 0.1, 0.0, 0.0); // top 
        EmitVertex();

        EndPrimitive();
    }
    else if(_activeGeom == 2)
    {
         fColor = gs_in[0].color;
        gl_Position = pos + vec4(-0.135, -0.65, 0.0, 0.0); // bottom left
        EmitVertex();

        gl_Position = pos + vec4(0.111, -0.876, 0.0, 0.0); // bottom right
        EmitVertex();

        gl_Position = pos + vec4(-0.45, 0.3, 0.0, 0.0); //top left
        EmitVertex();

        gl_Position = pos + vec4(0.11, 0.64, 0.0, 0.0); // top right 
        EmitVertex();

        gl_Position = pos + vec4(0.622, 0.76, 0.0, 0.0); // top 
        EmitVertex();

        EndPrimitive();
    }
    else if(_activeGeom == 1)
    {
        fColor = gs_in[0].color;
        gl_Position = pos + vec4(-0.8, -0.8, 0.0, 0.0); // bottom left
        EmitVertex();

        gl_Position = pos + vec4(0.8, -0.8, 0.0, 0.0); // bottom right
        EmitVertex();

        gl_Position = pos + vec4(-0.8, 0.8, 0.0, 0.0); //top left
        EmitVertex();

        gl_Position = pos + vec4(0.8, 0.8, 0.0, 0.0); // top right 
        EmitVertex();

        gl_Position = pos + vec4(0.0, 0.9, 0.0, 0.0); // top 
        EmitVertex();

        EndPrimitive();
    }
    else if(_activeGeom == 4)
    {
        fColor = gs_in[0].color;
        gl_Position = pos + vec4(-0.8, -0.8, 0.0, 0.0); // bottom left
        EmitVertex();

        gl_Position = pos + vec4(0.6, -0.6, 0.0, 0.0); // bottom right
        EmitVertex();

        gl_Position = pos + vec4(-0.8, 0.8, 0.0, 0.0); //top left
        EmitVertex();

        gl_Position = pos + vec4(0.8, 0.8, 0.0, 0.0); // top right 
        EmitVertex();

        gl_Position = pos + vec4(0.9, 0.9, 0.0, 0.0); // top 
        EmitVertex();

        EndPrimitive();
    }
}  
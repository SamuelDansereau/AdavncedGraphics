#version 450                          
out vec4 FragColor;

in vec2 _uv;

uniform int _post;
uniform int _effect;
uniform sampler2D _texture;
uniform float _time;

void main()
{
	vec4 finalCol =	texture(_texture, _uv);
	if(_post == 0)
	{
		FragColor = finalCol;
	}
	else 
	{
		if(_effect == 0)
		{
			float avg = (finalCol.x + finalCol.y + finalCol.z) / 3;
			finalCol = vec4(avg, avg, avg, 1);
			FragColor = finalCol;
		}
		else if(_effect == 1)
		{
			finalCol = vec4(1-finalCol.r, 1 - finalCol.g, 1 - finalCol.b, 1);
			FragColor = finalCol;
		}
		else if(_effect == 2)
		{
			vec2 uv = _uv;
			// I got most of this formula from https://en.wikibooks.org/wiki/OpenGL_Programming/Post-Processing but I flipped in time 
			uv.x += sin(uv.y * 4 *2*3.14 + _time) / 100;
			finalCol = texture(_texture, uv);
			FragColor = finalCol;
		}
		else if(_effect == 3)
		{
			vec2 uv = _uv;
			uv.y += sin(uv.x * 4 *2*3.14 + _time) / 100;
			finalCol = texture(_texture, uv);
			FragColor = finalCol;
		}
		else if(_effect == 4)
		{
			vec2 uv = _uv;
			uv.y += sin(uv.x * 4 *2*3.14 + _time) / 100;
			uv.x += sin(uv.y * 4 *2*3.14 + _time) / 100;
			finalCol = texture(_texture, uv);
			FragColor = finalCol;
		}
	}
}

#version 130

uniform sampler2D texture;
uniform float MIN_HEIGHT;

uniform float WIDTH;
uniform float HEIGHT;

float WIDTH_STEP = 1.0/WIDTH;
float HEIGHT_STEP= 1.0/HEIGHT;

void main()
{
	if (gl_FragCoord.y > MIN_HEIGHT)
	{
		vec4 color = texture2D(texture, vec2(gl_FragCoord.x*WIDTH_STEP, gl_FragCoord.y*HEIGHT_STEP));
		gl_FragData[0] = vec4(color.xyz, 1.0);
	}
}
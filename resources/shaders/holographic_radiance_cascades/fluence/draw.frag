/* Expected to be concatenated from CPU:



*/

//uniform sampler2DArray source;
uniform sampler2D fluence;

layout (location = 0) out vec4 color;

void main()
{
	color = texture(fluence, vec2(gl_FragCoord.xy / textureSize(fluence, 0)));
	//color = vec4(1.0, 0.0, 0.0, 1.0);
}

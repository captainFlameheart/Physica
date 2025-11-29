/* Expected to be concatenated from CPU:



*/

uniform sampler2DArray source;
uniform sampler2D fluence;

layout (location = 0) out vec4 color;

void main()
{
	// IMPORTANT TODO: Store inverse sizes to avoid division
	vec2 fluence_sample_point = gl_FragCoord.xy / vec2(textureSize(source, 0).xy);
	fluence_sample_point += (vec2(0.5) - fluence_sample_point) / vec2(textureSize(fluence, 0).xy);
	color = texture(fluence, fluence_sample_point);

	/////////////////////////////////////////////////////////////////////////////////////
	#if 1 == 0
		//color = vec4(1.0, 0.0, 0.0, 1.0);
	#endif
}

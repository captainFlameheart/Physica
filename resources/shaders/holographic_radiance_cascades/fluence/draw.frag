/* Expected to be concatenated from CPU:



*/

uniform sampler2DArray source;
uniform sampler2D fluence;

noperspective in vec2 sample_point;

layout (location = 0) out vec4 color;

void main()
{
	//vec2 sample_point = gl_FragCoord.xy / vec2(textureSize(source, 0).xy);
	color = texture(source, vec3(sample_point, 0.0));
	return;

	// IMPORTANT TODO: Store inverse size to avoid division
	// IMPORTANT TODO: Do this computation inside vertex shader
	vec2 fluence_sample_point = sample_point + (vec2(0.5) - sample_point) / vec2(textureSize(fluence, 0).xy);
	vec4 fluence_value = texture(fluence, fluence_sample_point);
	color = mix(color * (fluence_value + 0.001), fluence_value, 0.2);
}

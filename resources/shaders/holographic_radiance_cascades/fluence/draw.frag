/* Expected to be concatenated from CPU:

const vec2 source_sample_point_to_probe_grid_point_factor = vec2(?, ?);
const vec2 source_sample_point_to_probe_grid_point_bias = vec2(?, ?);

const vec2 probe_grid_point_to_fluence_sample_point_factor = vec2(?, ?);
const vec2 probe_grid_point_to_fluence_sample_point_bias = vec2(?, ?);

*/

uniform sampler2DArray source;
uniform sampler2D fluence;

noperspective in vec2 sample_point;

layout (location = 0) out vec4 color;

void main()
{
	//vec2 sample_point = gl_FragCoord.xy / vec2(textureSize(source, 0).xy);
	// VERY IMPORTANT TODO: Use texel fetch for source sampling when not zoomed out
	color = texture(source, vec3(sample_point, 0.0));

	// IMPORTANT TODO: Do this computation inside vertex shader.
	vec2 probe_grid_point = sample_point * source_sample_point_to_probe_grid_point_factor + source_sample_point_to_probe_grid_point_bias;
	vec2 fluence_sample_point = probe_grid_point * probe_grid_point_to_fluence_sample_point_factor + probe_grid_point_to_fluence_sample_point_bias; //sample_point + (vec2(0.5) - sample_point) / vec2(textureSize(fluence, 0).xy);
	vec4 fluence_value = texture(fluence, fluence_sample_point);
	color = mix(color * (fluence_value + 0.001), fluence_value, 0.2);
}

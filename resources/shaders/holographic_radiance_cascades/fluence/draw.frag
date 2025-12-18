/* Expected to be concatenated from CPU:

#define DEFAULT_ZOOM_MODE ?
#define ZOOMED_OUT_ZOOM_MODE ?

#define ZOOM_MODE ?

const vec2 source_sample_point_to_probe_grid_point_factor = vec2(?, ?);
const vec2 source_sample_point_to_probe_grid_point_bias = vec2(?, ?);

const vec2 probe_grid_point_to_fluence_sample_point_factor = vec2(?, ?);
const vec2 probe_grid_point_to_fluence_sample_point_bias = vec2(?, ?);

*/

uniform sampler2DArray source;
uniform sampler2D fluence;

#if ZOOM_MODE == ZOOMED_OUT_ZOOM_MODE
	noperspective in vec2 source_sample_point;
#endif
in vec2 fluence_sample_point;

layout (location = 0) out vec4 color;

void main()
{
	#if ZOOM_MODE == DEFAULT_ZOOM_MODE
		ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
		color = texelFetch(source, ivec3(output_texel_position, 0), 0);
	#elif ZOOM_MODE == ZOOMED_OUT_ZOOM_MODE
		color = texture(source, vec3(source_sample_point, 0.0), 0);
	#endif

	vec4 fluence_value = texture(fluence, fluence_sample_point);
	const vec4 ambience = vec4(0.001);
	fluence_value += ambience;
	
	color = mix(color * (fluence_value + fluence_value.a), fluence_value, color.a);
}

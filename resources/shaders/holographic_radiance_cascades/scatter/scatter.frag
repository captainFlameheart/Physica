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

layout (location = 0) out vec4 emission;

void main()
{
	#if ZOOM_MODE == DEFAULT_ZOOM_MODE
		ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
		emission = texelFetch(source, ivec3(output_texel_position, 3), 0);
	#elif ZOOM_MODE == ZOOMED_OUT_ZOOM_MODE
		emission = texture(source, vec3(source_sample_point, 3), 0);
	#endif

	emission *= texture(fluence, fluence_sample_point);
}

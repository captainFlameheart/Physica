/* Expected to be concatenated from CPU:

#define DEFAULT_ZOOM_MODE ?
#define ZOOMED_OUT_ZOOM_MODE ?

#define ZOOM_MODE ?

const vec2 source_sample_point_to_probe_grid_point_factor = vec2(?, ?);
const vec2 source_sample_point_to_probe_grid_point_bias = vec2(?, ?);

const vec2 probe_grid_point_to_fluence_sample_point_factor = vec2(?, ?);
const vec2 probe_grid_point_to_fluence_sample_point_bias = vec2(?, ?);

*/

layout(binding = source_image_texture_unit) uniform sampler2DArray source;
layout(binding = fluence_texture_unit) uniform sampler2D fluence;

#if ZOOM_MODE == ZOOMED_OUT_ZOOM_MODE
	noperspective in vec2 source_sample_point;
#endif
in vec2 fluence_sample_point;

layout (location = 0u) out vec4 color;

void main()
{
	vec4 unlit_color;
	#if ZOOM_MODE == DEFAULT_ZOOM_MODE
		ivec2 output_texel_position = ivec2(gl_FragCoord.xy);
		color = texelFetch(source, ivec3(output_texel_position, 0), 0);
		unlit_color = texelFetch(source, ivec3(output_texel_position, unlit_color_layer), 0);
	#elif ZOOM_MODE == ZOOMED_OUT_ZOOM_MODE
		color = texture(source, vec3(source_sample_point, 0.0), 0);
	#endif

	vec4 fluence_value = texture(fluence, fluence_sample_point);
	const float ambience = 0.05;
	fluence_value.a += ambience;
	
	vec4 reflected_color = color * fluence_value;

	vec4 radiatively_transferred_color_contribution = color * fluence_value.a;
	vec4 color_contribution = radiatively_transferred_color_contribution + unlit_color;

	color = mix(fluence_value, reflected_color, color.a) + color_contribution;
}

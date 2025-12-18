/* Expected to be concatenated from the CPU:

#define DEFAULT_ZOOM_MODE ?
#define ZOOMED_OUT_ZOOM_MODE ?

#define ZOOM_MODE ?

const vec2 source_sample_point_to_probe_grid_point_factor = vec2(?, ?);
const vec2 source_sample_point_to_probe_grid_point_bias = vec2(?, ?);

const vec2 probe_grid_point_to_fluence_sample_point_factor = vec2(?, ?);
const vec2 probe_grid_point_to_fluence_sample_point_bias = vec2(?, ?);

*/

const vec2 vertex_to_probe_grid_point_factor = 0.5 * source_sample_point_to_probe_grid_point_factor;
const vec2 vertex_to_probe_grid_point_bias = 0.5 * source_sample_point_to_probe_grid_point_factor + source_sample_point_to_probe_grid_point_bias;

const vec2 vertex_to_fluence_sample_point_factor = probe_grid_point_to_fluence_sample_point_factor * vertex_to_probe_grid_point_factor;
const vec2 vertex_to_fluence_sample_point_bias = probe_grid_point_to_fluence_sample_point_factor * vertex_to_probe_grid_point_bias + probe_grid_point_to_fluence_sample_point_bias;

#if ZOOM_MODE == DEFAULT_ZOOM_MODE
	const vec2 vertices[3u] = vec2[3u]
	(
		vec2(-1.0, -1.0), 
		vec2(3.0, -1.0), 
		vec2(-1.0, 3.0)
	);
#elif ZOOM_MODE == ZOOMED_OUT_ZOOM_MODE
	const vec2 vertices[6u] = vec2[6u]
	(
		vec2(-1.0, -1.0), 
		vec2(1.0, -1.0), 
		vec2(1.0, 1.0), 

		vec2(1.0, 1.0), 
		vec2(-1.0, 1.0), 
		vec2(-1.0, -1.0)
	);
#endif

#if ZOOM_MODE == ZOOMED_OUT_ZOOM_MODE
	noperspective out vec2 source_sample_point;
#endif
out vec2 fluence_sample_point;

void main()
{
	vec2 vertex = vertices[gl_VertexID];

	#if ZOOM_MODE == ZOOMED_OUT_ZOOM_MODE
		source_sample_point = vertex * 0.5 + 0.5;
	#endif

	fluence_sample_point = vertex * vertex_to_fluence_sample_point_factor + vertex_to_fluence_sample_point_bias;

	#if ZOOM_MODE == ZOOMED_OUT_ZOOM_MODE
		vertex *= 0.5;
	#endif
	gl_Position = vec4(vertex, 0.0, 1.0);
}

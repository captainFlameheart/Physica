/* Expected to be concatenated from the CPU:

#define DEFAULT_MODE ?
#define ZOOMED_OUT_MODE ?

#define MODE ?

*/

uniform uvec2 probe_grid_size;

out vec4 probe_color;

void main()
{
	uint row = gl_VertexID / probe_grid_size.x;
	uint column = gl_VertexID - row * probe_grid_size.x;

	const vec2 normalized_probe_distance = 2.0 / (probe_grid_size - 1u);
	gl_Position = vec4
	(
		column * normalized_probe_distance.x - 1.0,
		row * normalized_probe_distance.y - 1.0, 
		0.0, 1.0
	);

	#if MODE == ZOOMED_OUT_MODE
		gl_Position.xy *= 0.5;
	#endif

	const float brightness = 1.0;
	probe_color = vec4(brightness, brightness, brightness, 1.0);
}

/* Expected to be concatenated from the CPU:

#define DEFAULT_ZOOM_MODE ?
#define ZOOMED_OUT_ZOOM_MODE ?

#define ZOOM_MODE ?

*/

const vec2 vertices[6u] = vec2[6u]
(
	vec2(-1.0, -1.0), 
	vec2(1.0, -1.0), 
	vec2(1.0, 1.0), 

	vec2(1.0, 1.0), 
	vec2(-1.0, 1.0), 
	vec2(-1.0, -1.0)
);

noperspective out vec2 sample_point;

void main()
{
	vec2 vertex = vertices[gl_VertexID];
	sample_point = vertex * 0.5 + 0.5;
	#if MODE == ZOOMED_OUT_ZOOM_MODE
		vertex *= 0.5;
	#endif
	gl_Position = vec4(vertex, 0.0, 1.0);
}

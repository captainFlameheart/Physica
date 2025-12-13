/* Expected to be concatenated from the CPU:

#define PROJECTION_SCALE vec2(?, ?)

const float radius = ?;

*/

const vec2 vertices[6u] = vec2[6u]
(
	vec2(-radius, -radius), 
	vec2(radius, -radius), 
	vec2(radius, radius),

	vec2(radius, radius), 
	vec2(-radius, radius), 
	vec2(-radius, -radius)
);

noperspective out vec2 offset;

void main()
{
	offset = vertices[gl_VertexID];
	gl_Position = vec4(offset * PROJECTION_SCALE, 0.0, 1.0);
}

/* Expected to be concatenated from the CPU:



*/

vec2 vertices[6u] = vec2[6u]
(
	vec2(1.0, 1.0), 
	vec2(-1.0, 1.0), 
	vec2(-1.0, -1.0), 

	vec2(-1.0, -1.0), 
	vec2(1.0, -1.0), 
	vec2(1.0, 1.0)
);

layout(shared, binding = CURSOR_POSITION_BINDING) uniform Cursor_Position
{
	ivec2 position;
} cursor_position;

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

uniform float radius;

noperspective out vec2 offset;

void main()
{
	vec2 camera_relative_point_xy = cursor_position.position - camera.xy;
	gl_Position = vec4
	(
		camera.view_rotation * camera_relative_point_xy * PROJECTION_SCALE, 
		0.0, 
		camera.z
	);
	offset = vertices[gl_VertexID] * radius;
	gl_Position.xy += offset * PROJECTION_SCALE * camera.z;

}

/* Expected to be concatenated from the CPU:

const float cone_radius = ?;

*/

const float cone_radius_squared = cone_radius * cone_radius;

in vec2 offset;
in vec4 cone_color;

layout (location = 0) out vec4 color;

void main()
{
	if (dot(offset, offset) > cone_radius_squared)
	{
		discard;
	}
	color = cone_color;
}

/* Expected to be concatenated from the CPU:

const float radius = ?;
const float inner_radius = ?;

*/

const float radius_squared = radius * radius;
const float inner_radius_squared = inner_radius * inner_radius;

const float pi = 3.14159265358979323846;
const float angle_to_sky_circle_coordinate_factor = 0.5 / pi;

uniform sampler1D sky_circle;

noperspective in vec2 offset;

layout(location = 0) out vec4 color;

void main()
{
	float distance_squared = dot(offset, offset);
	if (any(bvec2(distance_squared < inner_radius_squared, radius_squared < distance_squared)))
	{
		discard;
	}

	float angle = atan(offset.y, offset.x);
	float sky_circle_coordinate = angle * angle_to_sky_circle_coordinate_factor;
	color = texture(sky_circle, sky_circle_coordinate);
	color.a = 1.0;
}

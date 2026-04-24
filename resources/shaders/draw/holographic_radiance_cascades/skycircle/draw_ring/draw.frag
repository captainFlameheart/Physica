const float angle_to_skycircle_coordinate_factor = 0.5 / pi;

layout(binding = skycircle_texture_unit) uniform sampler1D skycircle;

flat in vec2 radii;
noperspective in vec2 offset;

layout(location = 0u) out vec4 color;

void main()
{
	float distance_squared = dot(offset, offset);
	
	const float outer_radius_squared = radii.x * radii.x;
	const float inner_radius_squared = radii.y * radii.y;
	if (any(bvec2(distance_squared < inner_radius_squared, outer_radius_squared < distance_squared)))
	{
		discard;
	}

	float angle = atan(offset.y, offset.x);
	float skycircle_coordinate = angle * angle_to_skycircle_coordinate_factor;
	color = texture(skycircle, skycircle_coordinate);
}

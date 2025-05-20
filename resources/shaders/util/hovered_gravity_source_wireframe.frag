noperspective in vec2 offset;

layout (location = 0) out vec4 color;

void main()
{
    float distance_squared = dot(offset, offset);
    float min_distance = GRAB_RADIUS - 0.1 * METER;
    if (distance_squared < min_distance * min_distance || distance_squared > GRAB_RADIUS * GRAB_RADIUS)
    {
        discard;
    }
    color = vec4(1.0, 1.0, 0.0, 1.0);
}

noperspective in vec2 offset;

layout (location = 0) out vec4 color;

void main()
{
    float distance = length(offset);
    if (distance > GRAB_RADIUS + LIGHT_DISTANCE)
    {
        discard;
    }
    if (distance > GRAB_RADIUS)
    {
        float weight = (1.0 - (distance - (GRAB_RADIUS)) / (LIGHT_DISTANCE));
        color = vec4(1.0, 0.0, 1.0, 0.2 * weight * weight);
    }
    else if (distance > GRAB_RADIUS - 0.1 * METER)
    {
        color = vec4(1.0, 0.5, 1.0, 1.0);
    }
    else
    {
        float distance_fraction = distance / (GRAB_RADIUS);
        color = vec4(distance_fraction, 0.0, distance_fraction, 1.0);
    }
}

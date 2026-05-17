noperspective in vec2 offset;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 emission;
layout (location = 2) out vec4 attenuation;
layout (location = 3) out vec4 scattering;

void main()
{
    float distance_squared = dot(offset, offset);
    float min_distance = GRAB_RADIUS - 0.1 * METER;
    if (distance_squared < min_distance * min_distance || distance_squared > GRAB_RADIUS * GRAB_RADIUS)
    {
        discard;
    }
    color = vec4(1.0, 1.0, 0.0, 0.2);
    emission = vec4(color.rgb, 0.0);
    attenuation = vec4(0.0);
    scattering = vec4(0.0);
}

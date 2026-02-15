in vec2 offset;
in float radius;

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 emission;
layout(location = 2) out vec4 absorption;
layout(location = 3) out vec4 scattering;

void main()
{
    float offset_distance = length(offset);
    if (offset_distance > radius)
    {
        discard;
    }

    albedo = vec4(1.0, 1.0, 1.0, 0.2);
    emission = vec4(1.0, 1.0, 1.0, 0.0);
    absorption = vec4(1.0);
    scattering = vec4(0.0);
}

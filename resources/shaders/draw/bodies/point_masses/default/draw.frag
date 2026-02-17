in vec2 offset;
in vec4 color_radius;

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 emission;
layout(location = 2) out vec4 absorption;
layout(location = 3) out vec4 scattering;

void main()
{
    float offset_distance = length(offset);
    if (offset_distance > color_radius.a)
    {
        discard;
    }

    albedo = vec4(color_radius.rgb, 0.2);
    emission = vec4(color_radius.rgb, 0.0);
    absorption = vec4(1.0);
    scattering = vec4(0.0);
}

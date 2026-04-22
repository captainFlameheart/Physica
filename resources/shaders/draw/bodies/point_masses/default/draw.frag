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

    albedo = vec4(color_radius.rgb, default_reflectivity);
    emission = vec4(albedo.rgb * default_emission_scalar, 0.0);
    absorption = default_attenuation;
    scattering = default_scattering;
}

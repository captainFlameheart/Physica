in vec2 offset;
in float radius;
in vec4 material_albedo;
in vec4 material_emission;

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

    albedo = material_albedo;
    emission = material_emission;
    absorption = default_attenuation;
    scattering = default_scattering;
}

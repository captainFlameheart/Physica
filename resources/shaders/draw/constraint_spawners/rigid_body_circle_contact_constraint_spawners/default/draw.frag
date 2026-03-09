in vec4 color;

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 emission;
layout(location = 2) out vec4 absorption;
layout(location = 3) out vec4 scattering;

void main()
{
    albedo = color;
    emission = vec4(color.rgb, 0.0);
    absorption = vec4(1.0);
    scattering = vec4(0.0);
}

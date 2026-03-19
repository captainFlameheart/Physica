in vec2 UV;
flat in vec2 size;

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 emission;
layout(location = 2) out vec4 absorption;
layout(location = 3) out vec4 scattering;

void main()
{
    vec2 position_in_box = UV * size;
    float distance_from_edge = min
    (
        min(position_in_box.x, size.x - position_in_box.x),
        min(position_in_box.y, size.y - position_in_box.y)
    );
    const float stroke_width = 0.2 * meter_in_length_units;
    float is_inside_stroke_region = float(distance_from_edge < stroke_width);
    const float fill_alpha = 0.2;
    const float stroke_alpha = 1.0;
    const float alpha_difference = stroke_alpha - fill_alpha;
    float alpha = fill_alpha + is_inside_stroke_region * alpha_difference;
    
    albedo = vec4(1.0, 0.0, 0.0, alpha);
    emission = vec4(albedo.rgb, 0.0);
    absorption = vec4(1.0);
    scattering = vec4(0.0);
}

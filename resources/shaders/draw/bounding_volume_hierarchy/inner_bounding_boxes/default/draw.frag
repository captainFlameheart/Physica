#define DISCARD_CENTER 1

in vec4 color;
in vec2 UV;
flat in vec2 size;

layout(location = unlit_color_layer) out vec4 unlit_color;

void main()
{
    vec2 position_in_box = UV * size;
    float distance_from_edge = min
    (
        min(position_in_box.x, size.x - position_in_box.x),
        min(position_in_box.y, size.y - position_in_box.y)
    );
    const float stroke_width = 0.1 * meter_in_length_units;
    #if DISCARD_CENTER == 1
        if (distance_from_edge >= stroke_width)
        {
            discard;
        }
        unlit_color = color;
    #else
        float is_inside_stroke_region = float(distance_from_edge < stroke_width);

        const float fill_alpha = 0.2;
        const float stroke_alpha = 1.0;
        const float alpha_difference = stroke_alpha - fill_alpha;
        float alpha = fill_alpha + is_inside_stroke_region * alpha_difference;
    
        unlit_color = vec4(color.rgb, alpha);
    #endif
}

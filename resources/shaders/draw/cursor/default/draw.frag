#define DISCARD_CENTER 1

in vec2 UV;
flat in uint icon_base;

layout(location = emission_layer) out vec4 emission;
layout(location = attenuation_layer) out vec4 attenuation;
layout(location = unlit_color_layer) out vec4 unlit_color;

void main()
{
    const uint max_icon_coordinate = cursor_icon_length - 1u;
    uint icon_row_index = min(uint(UV.y * cursor_icon_length), max_icon_coordinate);
    uint icon_column_index = min(uint(UV.x * cursor_icon_length), max_icon_coordinate);

    uint icon_row = fixed_data.cursor_icons[icon_base + icon_row_index];
    uint bit_index = max_icon_coordinate - icon_column_index;
    uint icon_fill_bit    = (icon_row >> bit_index) & 1u;
    uint icon_outline_bit = (icon_row >> (bit_index + 16u)) & 1u;

    if (icon_fill_bit + icon_outline_bit == 0u)
    {
        discard;
    }

    emission = vec4(0.0);
    attenuation = default_attenuation;
    unlit_color = vec4(icon_fill_bit);
}

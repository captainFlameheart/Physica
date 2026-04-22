#define DISCARD_CENTER 1

in vec2 UV;
flat in uint icon_base;

layout(location = unlit_color_layer) out vec4 unlit_color;

void main()
{
    uint icon_row_index = min(uint(UV.y * cursor_icon_length), cursor_icon_length);
    uint icon_column_index = min(uint(UV.x * cursor_icon_length), cursor_icon_length);

    uint icon_row = fixed_data.cursor_icons[icon_base + icon_row_index];
    uint icon_bit = (icon_row >> icon_column_index) & 1u;

    if (icon_bit == 0u)
    {
        discard;
    }

    unlit_color = vec4(1.0);
}

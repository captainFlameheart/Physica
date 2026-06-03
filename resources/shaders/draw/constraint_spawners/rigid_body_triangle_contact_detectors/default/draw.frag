in vec4 color;

layout(location = unlit_color_layer) out vec4 unlit_color;

void main()
{
    unlit_color = color;
}

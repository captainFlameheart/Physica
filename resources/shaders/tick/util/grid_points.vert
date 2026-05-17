#define X_STEP (STEP * PROJECTION_SCALE.x)
#define Y_STEP (STEP * PROJECTION_SCALE.y)
#define WIDTH (1u + 2u * uint(1.0 / X_STEP))
#define HEIGHT (1u + 2u * uint(1.0 / Y_STEP))

layout(shared, binding = CAMERA_BINDING) uniform Camera
{
	ivec2 xy;
	int angle;
	float z;
	mat2 view_rotation;
} camera;

void main()
{
	uint x_index = gl_VertexID % WIDTH;
	float x_sign = 2.0 * float(x_index % 2u) - 1.0;
	float x_abs = float((x_index + 1u) / 2u) * X_STEP;

	uint y_index = gl_VertexID / WIDTH;
	float y_sign = (2.0 * float(y_index % 2u) - 1.0);
	float y_abs = float((y_index + 1u) / 2u) * Y_STEP;

	float x = x_sign * x_abs;
	float y = y_sign * y_abs;

	gl_Position = vec4(x, y, 0.0, 1.0);
}

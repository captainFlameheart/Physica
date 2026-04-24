#include "game_logic/initialize/compile_shaders/environment/include.h"
#include "game_environment/environment.h"
#include "game_logic/shader_group/include.h"
#include "game_state/device_requirements/OpenGL_versions.h"
#include "util/shader/shader.h"
#include "game_state/bindings/include.h"
#include "game_state/local_sizes/include.h"
#include "game_state/vertex_factors/include.h"
#include "game_state/entity_type_indices/entity_type_indices.h"
#include "game_state/shader_to_entity_type/shader_to_entity_type.h"
#include "game_state/units/include.h"
#include "macros/macros.h"
#include <iostream>

namespace game_logic::initialize::compile_shaders::environment
{
	std::string initialize_input_constants
	(
		game_environment::Environment& environment
	)
	{
		return (
			"const uint press = " STRINGIFY(GLFW_PRESS) ";\n"
			"const uint release = " STRINGIFY(GLFW_RELEASE) ";\n"
			"const uint repeat = " STRINGIFY(GLFW_REPEAT) ";\n"

			"const uint key_space = " STRINGIFY(GLFW_KEY_SPACE) ";\n"
			
			"const uint key_0 = " STRINGIFY(GLFW_KEY_0) ";\n"
			"const uint key_1 = " STRINGIFY(GLFW_KEY_1) ";\n"
			"const uint key_2 = " STRINGIFY(GLFW_KEY_2) ";\n"
			"const uint key_3 = " STRINGIFY(GLFW_KEY_3) ";\n"
			"const uint key_4 = " STRINGIFY(GLFW_KEY_4) ";\n"
			"const uint key_5 = " STRINGIFY(GLFW_KEY_5) ";\n"
			"const uint key_6 = " STRINGIFY(GLFW_KEY_6) ";\n"
			"const uint key_7 = " STRINGIFY(GLFW_KEY_7) ";\n"
			"const uint key_8 = " STRINGIFY(GLFW_KEY_8) ";\n"
			"const uint key_9 = " STRINGIFY(GLFW_KEY_9) ";\n"

			"const uint key_a = " STRINGIFY(GLFW_KEY_A) ";\n"
			"const uint key_b = " STRINGIFY(GLFW_KEY_B) ";\n"
			"const uint key_c = " STRINGIFY(GLFW_KEY_C) ";\n"
			"const uint key_d = " STRINGIFY(GLFW_KEY_D) ";\n"
			"const uint key_e = " STRINGIFY(GLFW_KEY_E) ";\n"
			"const uint key_f = " STRINGIFY(GLFW_KEY_F) ";\n"
			"const uint key_g = " STRINGIFY(GLFW_KEY_G) ";\n"
			"const uint key_h = " STRINGIFY(GLFW_KEY_H) ";\n"
			"const uint key_i = " STRINGIFY(GLFW_KEY_I) ";\n"
			"const uint key_j = " STRINGIFY(GLFW_KEY_J) ";\n"
			"const uint key_k = " STRINGIFY(GLFW_KEY_K) ";\n"
			"const uint key_l = " STRINGIFY(GLFW_KEY_L) ";\n"
			"const uint key_m = " STRINGIFY(GLFW_KEY_M) ";\n"
			"const uint key_n = " STRINGIFY(GLFW_KEY_N) ";\n"
			"const uint key_o = " STRINGIFY(GLFW_KEY_O) ";\n"
			"const uint key_p = " STRINGIFY(GLFW_KEY_P) ";\n"
			"const uint key_q = " STRINGIFY(GLFW_KEY_Q) ";\n"
			"const uint key_r = " STRINGIFY(GLFW_KEY_R) ";\n"
			"const uint key_s = " STRINGIFY(GLFW_KEY_S) ";\n"
			"const uint key_t = " STRINGIFY(GLFW_KEY_T) ";\n"
			"const uint key_u = " STRINGIFY(GLFW_KEY_U) ";\n"
			"const uint key_v = " STRINGIFY(GLFW_KEY_V) ";\n"
			"const uint key_w = " STRINGIFY(GLFW_KEY_W) ";\n"
			"const uint key_x = " STRINGIFY(GLFW_KEY_X) ";\n"
			"const uint key_y = " STRINGIFY(GLFW_KEY_Y) ";\n"
			"const uint key_z = " STRINGIFY(GLFW_KEY_Z) ";\n"

			"const uint key_right = " STRINGIFY(GLFW_KEY_RIGHT) ";\n"
			"const uint key_left = " STRINGIFY(GLFW_KEY_LEFT) ";\n"
			"const uint key_down = " STRINGIFY(GLFW_KEY_DOWN) ";\n"
			"const uint key_up = " STRINGIFY(GLFW_KEY_UP) ";\n"

			"const uint key_left_shift = " STRINGIFY(GLFW_KEY_LEFT_SHIFT) ";\n"
			"const uint key_left_control = " STRINGIFY(GLFW_KEY_LEFT_CONTROL) ";\n"

			"const uint key_menu = " STRINGIFY(GLFW_KEY_MENU) ";\n"
			"const uint key_last = " STRINGIFY(GLFW_KEY_LAST) ";\n"
			
			"const uint key_count = " STRINGIFY(GLFW_KEY_LAST + 1) ";\n"

			"const uint mouse_button_1 = " STRINGIFY(GLFW_MOUSE_BUTTON_1) ";\n"
			"const uint mouse_button_2 = " STRINGIFY(GLFW_MOUSE_BUTTON_2) ";\n"
			"const uint mouse_button_3 = " STRINGIFY(GLFW_MOUSE_BUTTON_3) ";\n"
			"const uint mouse_button_4 = " STRINGIFY(GLFW_MOUSE_BUTTON_4) ";\n"
			"const uint mouse_button_5 = " STRINGIFY(GLFW_MOUSE_BUTTON_5) ";\n"
			"const uint mouse_button_6 = " STRINGIFY(GLFW_MOUSE_BUTTON_6) ";\n"
			"const uint mouse_button_7 = " STRINGIFY(GLFW_MOUSE_BUTTON_7) ";\n"
			"const uint mouse_button_8 = " STRINGIFY(GLFW_MOUSE_BUTTON_8) ";\n"
			"const uint mouse_button_last = " STRINGIFY(GLFW_MOUSE_BUTTON_LAST) ";\n"
			"const uint mouse_button_left = " STRINGIFY(GLFW_MOUSE_BUTTON_LEFT) ";\n"
			"const uint mouse_button_right = " STRINGIFY(GLFW_MOUSE_BUTTON_RIGHT) ";\n"
			"const uint mouse_button_middle = " STRINGIFY(GLFW_MOUSE_BUTTON_MIDDLE) ";\n"

			"const uint mouse_button_count = " STRINGIFY(GLFW_MOUSE_BUTTON_LAST + 1) ";\n"
		);
	}
}

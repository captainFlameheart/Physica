#include "game_logic/initialize/query_layouts/include.h"
#include "game_environment/Environment.h"

namespace game_logic::initialize::query_layouts
{
	void query_layouts(game_environment::Environment& environment)
	{
		query_fixed_data_layout(environment);
		query_uvec4_data_layout(environment);
		query_uint_data_layout(environment);
	}
}

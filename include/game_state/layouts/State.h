#pragma once
#include "bounding_volume_hierarchy/include.h"
#include "commands/include.h"
#include "fixed_data/include.h"
#include "float_data/include.h"
#include "key_event/include.h"
#include "merge_fluence_data/include.h"
#include "merge_rays_data/include.h"
#include "uint_data/include.h"
#include "uvec4_data/include.h"
#include "uvec2_data/include.h"
#include "key_event/include.h"
#include "vec2_data/include.h"

namespace game_state::layouts
{
	struct State
	{
		fixed_data::State fixed_data;
		uvec4_data::Aliased_State uvec4_data;
		uvec2_data::Aliased_State uvec2_data;
		uint_data::Aliased_State uint_data;
		vec4_data::Aliased_State vec4_data;
		vec2_data::Aliased_State vec2_data;
		float_data::Aliased_State float_data;

		commands::State commands;

		key_event::State key_event;
		
		merge_rays_data::State merge_rays_data;
		merge_fluence_data::State merge_fluence_data;

		bounding_volume_hierarchy::State bounding_volume_hierarchy;
	};
}

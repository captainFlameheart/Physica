#include "positions/include.h"
#include "velocities/include.h"
#include "triangles/include.h"

namespace game_state::GPU_buffers::rigid_bodies
{
	struct Buffer
	{
		positions::Buffer positions;
		velocities::Buffer velocities;
		triangles::Buffer triangles;
	};
}

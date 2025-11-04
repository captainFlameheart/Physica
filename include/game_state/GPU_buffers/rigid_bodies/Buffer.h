#include "positions/include.h"
#include "velocities/include.h"
#include "triangles/include.h"
#include "distance_constraints/include.h"
#include "masses/include.h"

namespace game_state::GPU_buffers::rigid_bodies
{
	struct Buffer
	{
		// TODO: Position buffer should likely come after velocity buffer
		positions::Buffer positions;
		velocities::Buffer velocities;
		triangles::Buffer triangles;
		distance_constraints::Buffer distance_constraints;
		masses::Buffer masses;
	};
}

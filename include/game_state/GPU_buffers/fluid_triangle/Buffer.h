#include "contacts/include.h"
#include "contact_count/include.h"

namespace game_state::GPU_buffers::fluid_triangle
{
	struct Buffer
	{
		contacts::Buffer contacts;
		contact_count::Buffer contact_count;
	};
}

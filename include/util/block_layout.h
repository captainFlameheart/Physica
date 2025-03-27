#pragma once
#include <glad/glad.h>
#include <string>

//
// block_layout::get
// (
//	game_state.state.camera_block_layout.size,
//	"vec4 transform", game_state.state.camera_block_layout.transform_offset
//	"float", 4, game_state.state.camera_block_layout.float_array, stride,  
// )

namespace block_layout
{
	template <typename ...MemberQueries>
	void get(std::string const &representation_path, std::string const &block_ GLint &size, MemberQueries... member_queries);
}

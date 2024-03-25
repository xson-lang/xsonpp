#pragma once
#include <cstddef>
#include <cstdint>
#include <array>
#include <vector>

#include "string.hpp"

namespace xson {
	template<std::size_t N>
	using array = std::array<string, N>;

	using list = std::vector<string>; 
}

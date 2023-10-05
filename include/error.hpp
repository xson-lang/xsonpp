#pragma once
#include <array>
#include <string_view>
#include <cstdint>

namespace xson {
	enum class error_code {


		num_codes
	};


	constexpr std::array<std::string_view, static_cast<std::size_t>(error_code::num_codes)> error_msgs = {

	};
}


namespace xson {
	template<error_code Code>
	struct error {
		constexpr static error_code code = Code;
		constexpr static std::string_view desc = error_msgs[static_cast<std::size_t>(Code)];
	};
}
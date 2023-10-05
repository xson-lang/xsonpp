#include "result.hpp"

namespace xson {
	constexpr result::result(error_code code) noexcept :
		code(code) {};


	constexpr std::string_view result::desc() noexcept {
		return code < error_code::num_codes ? error_msgs[static_cast<std::size_t>(Code)] : "<no description for unknown error code>";
	}

	constexpr bool result::success() noexcept {
		return code == error_code::none;
	}
	constexpr result::operator bool() noexcept { return success(); }
}
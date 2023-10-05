#pragma once
#include <array>
#include <string_view>
#include <cstdint>

namespace xson {
	enum class error_code : std::size_t {
		none,



		num_codes
	};


	constexpr std::array<std::string_view, static_cast<std::size_t>(error_code::num_codes)> error_msgs = {

	};
}


namespace xson {
	struct result {
		constexpr result() noexcept = default;
		constexpr result(error_code code) noexcept;

		constexpr std::string_view desc() noexcept;

		constexpr bool success() noexcept;
		constexpr operator bool() noexcept;

		error_code code = error_code::none;
	};
}

#include "../src/result.inl"
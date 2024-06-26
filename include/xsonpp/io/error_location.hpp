#pragma once
#include <cstddef>
#include <string_view>

namespace xson{
	struct error_location {
		std::size_t line = 1, col = 1;
		std::string_view file = {};
        
		constexpr void next_line() noexcept;
		constexpr void next(char curr_char) noexcept;
        
		constexpr error_location& operator++() noexcept;
		constexpr error_location operator++(int) noexcept;
		constexpr error_location& operator+=(std::size_t rhs) noexcept;
		friend constexpr error_location operator+(error_location lhs, std::size_t rhs) noexcept;
		friend constexpr error_location operator+(std::size_t lhs, error_location rhs) noexcept;
	};
}

#include "../src/xsonpp/io/error_location.inl"
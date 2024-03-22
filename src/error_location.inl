#pragma once
#include "XsonCpp/io/error_location.hpp"

namespace xson {
	constexpr void error_location::next_line() noexcept { 
        ++line; 
        col = 0; 
    }

	constexpr void error_location::next(char curr_char) noexcept {
        if(curr_char == '\n') return next_line();

        ++(*this);
    }
}

namespace xson {
	constexpr error_location& error_location::operator++() noexcept { 
        ++col; 
        return *this; 
    }
	
    constexpr error_location error_location::operator++(int) noexcept { 
        return { line, col++ }; 
    }
	
    constexpr error_location& error_location::operator+=(std::size_t rhs) noexcept { 
        col += rhs; 
        return *this; 
    }


	constexpr error_location operator+(error_location lhs, std::size_t rhs) noexcept { 
        lhs += rhs; 
        return lhs; 
    }

	constexpr error_location operator+(std::size_t lhs, error_location rhs) noexcept { 
        rhs += lhs; 
        return rhs; 
    }
}
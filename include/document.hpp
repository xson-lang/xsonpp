#pragma once
#include <unordered_map>
#include <array>
#include <type_traits>
#include <cstdint>
#include <string_view>
#include <uni_str/uni_str.hpp>

#include "type_traits.hpp"

namespace xson {
	using string = ol::uni_string<ol::UTF8<>>;


	template<std::size_t N>
	using array = std::array<string, N>;
}

namespace xson {
	struct object {
		//default case
		template<typename T = string>
		std::enable_if_t<impl::is_not_specialized_v<T>, std::decay_t<T>>
		get(std::string_view key_name);
		

		//specialization for array-like types (i.e. non-string-like types with a subscript operator)
		template<typename T>
		std::enable_if_t<impl::is_array_like_v<T>, std::decay_t<T>>
		get(std::string_view key_name);
		
		
		//specialization for numeric types
		template<typename T>
		std::enable_if_t<std::is_arithmetic_v<impl::remove_cvref_t<T>>, std::decay_t<T>>
		get(std::string_view key_name);
		
		
		//specialization for objects
		template<typename T>
		std::enable_if_t<std::is_convertible_v<T, object>, object>
		get(std::string_view key_name);
	private:
		std::unordered_map<string, string> data;
		std::unordered_map<string, object> objects;

		friend struct reader;
	};

	using document = object; 
}
#pragma once
#include <type_traits>
#include <map>
#include <string_view>

#include "type_traits.hpp"
#include "string.hpp"

namespace xson {
	struct object {
		//default case
		template<typename T = string>
		std::enable_if_t<impl::is_not_specialized_v<T>, std::decay_t<T>>
		get(std::string_view key_name) const noexcept;
		

		//specialization for array-like types (i.e. non-string-like types with a subscript operator)
		template<typename T>
		std::enable_if_t<impl::is_array_like_v<T>, std::decay_t<T>>
		get(std::string_view key_name) const noexcept;
		
		
		//specialization for numeric types
		template<typename T>
		std::enable_if_t<std::is_arithmetic_v<impl::remove_cvref_t<T>>, std::decay_t<T>>
		get(std::string_view key_name) const noexcept;
		
		
		//specialization for objects
		template<typename T>
		std::enable_if_t<std::is_convertible_v<T, object>, object>
		get(std::string_view key_name) const noexcept;
	private:
		friend struct parser;
		std::map<string, string> kv_pairs;
	};
}
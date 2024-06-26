#pragma once
#include <type_traits>
#include <map>
#include <unordered_map>
#include <string_view>
#include <charconv>
#include <zsimd/attrib/flatten.h>
#include <zstring.hpp>

#include "xsonpp/xson/ext_list.hpp"
#include "xsonpp/xson/value/type_traits.hpp"
#include "xsonpp/xson/value/string.hpp"
#include "xsonpp/error/info.hpp"

namespace xson {
	template<template<typename...> typename KVMapTy = std::map>
	struct object {
		constexpr object() noexcept = default;
		inline object(ext_list exts, KVMapTy<string, string> kv_map) noexcept : enabled_exts(exts), kv_pairs(kv_map) {}

		//default case
		template<typename T = string>
		std::enable_if_t<impl::is_not_specialized_v<T, KVMapTy>, result<std::decay_t<T>>>
		get(std::string_view key_name) const noexcept;
		

		//specialization for array-like types (i.e. non-string-like types with a subscript operator)
		template<typename T> ZSIMD_FLATTEN
		std::enable_if_t<impl::is_array_like_v<T>, result<std::decay_t<T>>>
		get(std::string_view key_name) const noexcept;
		
		
		//specialization for floating types
		template<typename T>
		std::enable_if_t<std::is_floating_point_v<impl::remove_cvref_t<T>>, result<std::decay_t<T>>>
		get(std::string_view key_name, std::chars_format fmt = std::chars_format::general) const noexcept;


		//specialization for integer types
		template<typename T>
		std::enable_if_t<std::is_integral_v<impl::remove_cvref_t<T>>, result<std::decay_t<T>>>
		get(std::string_view key_name, int base = 10) const noexcept;
		
		
		//specialization for objects
		template<typename T>
		std::enable_if_t<std::is_convertible_v<T, object>, result<object>>
		get(std::string_view key_name) const noexcept;


	public:
		constexpr ext_list exts() const noexcept { return enabled_exts; };


	private:
		ext_list enabled_exts;
		KVMapTy<string, string> kv_pairs;
	};


	using small_object = object<std::map>;
	using large_object = object<std::unordered_map>;
}


#include "../src/xsonpp/xson/get.inl"
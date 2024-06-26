#pragma once
#include "xsonpp/error/code.hpp"
#include "xsonpp/xson/value/object.hpp"

#include <charconv> //TODO replace with zsimd charconv
#include <result.hpp>
#include <string_view>

#include "xsonpp/io/parser.hpp"
#include "xsonpp/xson/value/type_traits.hpp"

namespace xson{
	//default case
	template<template<typename...> typename KVMapTy>
	template<typename T>
	std::enable_if_t<impl::is_not_specialized_v<T, KVMapTy>, result<std::decay_t<T>>>
	object<KVMapTy>::get(std::string_view key_name) const noexcept {
        if (auto kv = kv_pairs.find(string{key_name}); kv != kv_pairs.end()) 
            return static_cast<std::decay_t<T>>(kv->second);
        return error::info{error::code::key_not_found};
    }
	

	//specialization for array-like types (i.e. non-string-like types with a subscript operator)
	template<template<typename...> typename KVMapTy>
	template<typename T> ZSIMD_FLATTEN
	std::enable_if_t<impl::is_array_like_v<T>, result<std::decay_t<T>>>
	object<KVMapTy>::get(std::string_view key_name) const noexcept {
		auto kv = kv_pairs.find(string{key_name});
        if (kv == kv_pairs.end()) 
            return error::info{error::code::key_not_found};

		std::string val = static_cast<std::string>(kv->second); //TODO data() function 
		if(val.size() < 2 || val[0] != '[' || val[val.size() - 1] != ']')
			return error::info{std::errc::invalid_argument};
		if(val.size() == 2) 
			return T{};

		std::string cs_vals{val.data() + 1, val.size() - 2}; 

		T ret{};
		constexpr static char whitespace_chars[6] = ",\r\n\t ";
		std::size_t idx = zsimd::algo::strspn(cs_vals.data(), whitespace_chars, cs_vals.length());
		std::size_t i = 0;
		while(idx < cs_vals.size()) {
			std::size_t len = zsimd::algo::strcspn(cs_vals.data() + idx, whitespace_chars, cs_vals.length() - idx); //TODO make whitespace characters global
			if constexpr (impl::has_emplace_back_v<T, std::string>)
				ret.emplace_back(cs_vals.substr(idx, len));
			else 
				ret[i++] = cs_vals.substr(idx, len);
			idx += len + 1;
			idx += zsimd::algo::strspn(cs_vals.data() + idx, whitespace_chars, cs_vals.length() - idx);
		}
		return ret;
    }
	

	//specialization for floating types
	template<template<typename...> typename KVMapTy>
	template<typename T>
	std::enable_if_t<std::is_floating_point_v<impl::remove_cvref_t<T>>, result<std::decay_t<T>>>
	object<KVMapTy>::get(std::string_view key_name, std::chars_format fmt) const noexcept {
		auto kv = kv_pairs.find(string{key_name});
        if (kv == kv_pairs.end()) 
            return error::info{error::code::key_not_found};

		std::string val = static_cast<std::string>(kv->second); //TODO add data() function to uni_str (and then change default encoding for xson to char)?
		T ret{};
		std::from_chars_result res = std::from_chars(val.data(), val.data() + val.size(), ret, fmt);
		if (res.ec == std::errc()) return ret;
		return error::info{res.ec};
    }


	//specialization for integer types
	template<template<typename...> typename KVMapTy>
	template<typename T>
	std::enable_if_t<std::is_integral_v<impl::remove_cvref_t<T>>, result<std::decay_t<T>>>
	object<KVMapTy>::get(std::string_view key_name, int base) const noexcept {
		auto kv = kv_pairs.find(string{key_name});
        if (kv == kv_pairs.end()) 
            return error::info{error::code::key_not_found};

		std::string val = static_cast<std::string>(kv->second); //TODO add data() function to uni_str (and then change default encoding for xson to char)?
		T ret{};
		std::from_chars_result res = std::from_chars(val.data(), val.data() + val.size(), ret, base);
		if (res.ec == std::errc()) return ret;
		return error::info{res.ec};
    }
	
	
	//specialization for objects
	template<template<typename...> typename KVMapTy>
	template<typename T>
	std::enable_if_t<std::is_convertible_v<T, object<KVMapTy>>, result<object<KVMapTy>>>
	object<KVMapTy>::get(std::string_view key_name) const noexcept {
		auto kv = kv_pairs.find(string{key_name});
        if (kv == kv_pairs.end()) 
            return error::info{error::code::key_not_found};

		std::string val = static_cast<std::string>(kv->second); //TODO data() function 
		if(val.size() < 2 || val[0] != '{' || val[val.size() - 1] != '}')
			return error::info{std::errc::invalid_argument};
		if(val.size() == 2) 
			return object<KVMapTy>{enabled_exts, {}};

		//TODO in zsimd make an allocator for std::string and/or sprintf-like function
		constexpr static std::string_view loc_left = "<value of '", loc_right = "'>";
		std::string loc(loc_left.size() + loc_right.size() + key_name.size() + 1, '\0');
		zsimd::algo::memcpy(loc.data(), loc_left.data(), loc_left.size());
		zsimd::algo::memcpy(loc.data() + loc_left.size(), key_name.data(), key_name.size());
		zsimd::algo::memcpy(loc.data() + loc_left.size() + key_name.size(), loc_right.data(), loc_right.size());

		parser p{enabled_exts, {1, 1, loc}};
		auto r = p.parse<KVMapTy>(val.data() + 1, val.size() - 2);
		if(!r.has_value())
			return r.error();
		return {enabled_exts, *r};
    }
}
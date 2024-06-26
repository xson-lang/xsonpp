#pragma once
#include <map>
#include <optional>
#include <result.hpp>
#include <type_traits>

#include "error_location.hpp"
#include "xsonpp/xson/ext_list.hpp"
#include "xsonpp/xson/segment.hpp"
#include "xsonpp/error/code.hpp"
#include "xsonpp/error/info.hpp"
#include "xsonpp/xson/value/string.hpp"


namespace xson {
    struct parser {
		using char_type = char;

	public:
		constexpr parser(ext_list enabled_exts, error_location curr_err_loc = {}) noexcept : exts(enabled_exts), err_loc(curr_err_loc) {}

		template<segment::category S> 
		segment::result_type_t<S> parse(const char_type* const char_ptr, std::size_t length) noexcept;

		template<template<typename...> class KVMapTy = std::map> 
		result<KVMapTy<string, string>> parse(const char_type* const char_ptr, std::size_t length) noexcept;

	private:
		error::info make_parse_error(error::code value, std::optional<error_location> l = std::nullopt) const noexcept;

	private:
		ext_list exts{true << 0};
		error_location err_loc;
    };	
}


namespace xson {
	template<>
	segment::result_type_t<segment::comment> parser::parse<segment::comment>(const parser::char_type* const char_ptr, std::size_t length) noexcept;

	template<>
	segment::result_type_t<segment::directive> parser::parse<segment::directive>(const parser::char_type* const char_ptr, std::size_t length) noexcept;
}

#ifdef XSONPP_HEADER_ONLY
#define _INLINE_HEADER_ONLY inline
#include "../src/xsonpp/io/parser.ipp"
#else
#define _INLINE_HEADER_ONLY
#endif

#include "../src/xsonpp/io/parser/object.inl"
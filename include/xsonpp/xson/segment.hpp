#pragma once
#include <cstdint>
#include <type_traits>
#include <utility>

#include "value/array.hpp"
#include "ext_list.hpp"
#include "../result/result.hpp"

#define COMMA ,
#define XSONPP_SEGMENTS_NON_CONTENT \
X(directive,         result<std::pair<ext_list COMMA std::size_t>>, '#') \
X(comment,           std::size_t     , '/') \
X(comment_multiline, std::size_t     , '*') \

#define XSONPP_SEGMENTS_CONTENT \
X(object, result<struct object>, '{', '}') \
X(array,  result<list>,          '[', ']') \

#define XSONPP_SEGMENTS XSONPP_SEGMENTS_NON_CONTENT XSONPP_SEGMENTS_CONTENT


namespace xson {
    struct segment {
        enum category : std::uint_fast8_t {
            none = false,
            #define X(name, ...) name,
			XSONPP_SEGMENTS
            #undef X
		};
	    
        using value_type = std::underlying_type_t<category>;


	    template<category S> struct opening_delim;
        template<category S> struct closing_delim;
        template<category S> struct delim;

        #define X(name, _, open, close) \
        template<> struct opening_delim<name> : std::integral_constant<value_type, open> {}; \
        template<> struct closing_delim<name> : std::integral_constant<value_type, close> {};
        XSONPP_SEGMENTS_CONTENT
        #undef X
        #define X(name, _, dlim) \
        template<> struct delim<name> : std::integral_constant<value_type, dlim> {};
        XSONPP_SEGMENTS_NON_CONTENT
        #undef X

        template<category S> constexpr static value_type opening_delim_v = opening_delim<S>::value;
        template<category S> constexpr static value_type closing_delim_v = closing_delim<S>::value;
        template<category S> constexpr static value_type delim_v = delim<S>::value;


	    template<category S> struct result_type;

        #define X(name, ty, ...) \
        template<> struct result_type<name> { using type = ty; }; 
        XSONPP_SEGMENTS
        #undef X

	    template<category S> using result_type_t = typename result_type<S>::type;
    };
}

#undef COMMA
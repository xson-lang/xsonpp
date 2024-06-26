#pragma once
#include <cstdint>
#include <type_traits>
#include <result.hpp>

#include "xsonpp/error/info.hpp"
#include "xsonpp/xson/value/array.hpp"
#include "xsonpp/xson/value/string.hpp"
#include "xsonpp/xson/ext_list.hpp"

#define COMMA ,
#define XSONPP_SEGMENTS \
X(directive,         result<std::pair<ext_list COMMA std::size_t>>, '#') \
X(comment,           std::size_t     , '/') \
X(comment_multiline, std::size_t     , '*') \


namespace xson {
    struct segment {
        enum category : std::uint_fast8_t {
            none = false,
            #define X(name, ...) name,
			XSONPP_SEGMENTS
            #undef X
		};
	    
        using value_type = std::underlying_type_t<category>;


        template<category S> struct delim;
        
        #define X(name, _, dlim) \
        template<> struct delim<name> : std::integral_constant<value_type, dlim> {};
        XSONPP_SEGMENTS
        #undef X

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
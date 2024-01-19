#pragma once
#include <bitset>
#include <cstdint>
#include <unordered_map>
#include <string_view>

#include "exts.hpp"


namespace xson {
	namespace impl {
		template<std::size_t... Ids>
		constexpr ext_list ext_set = { ((1 << Ids) | ...) };
		//struct ext_set {
		//	constexpr static ext_list list{ (1 << Ids | ...) };
		//};
	}
}


#define XSONCPP_BASE_EXT_SETS \
X(ini,  2, 6, 8, 9) \
X(json, 3, 5, 6, 7) \
X(toml, 2, 3, 6, 8) \

#define XSONCPP_EXT_SETS 

namespace xson {
	namespace ext_set {
		#define X(name, ...) \
		constexpr ext_list name = impl::ext_set<__VA_ARGS__>;

		XSONCPP_BASE_EXT_SETS
		XSONCPP_EXT_SETS
		#undef X

		/*constexpr ext_list ini  = impl::ext_set<2, 6, 8, 9>;
		constexpr ext_list json = impl::ext_set<3, 5, 6, 7>;
		constexpr ext_list toml = impl::ext_set<2, 3, 6, 8>;*/
	}

	
	namespace ext_set {
		//TODO constexpr/compile-time map
		const std::unordered_map<std::string_view, ext_list> from_name = {
			#define X(name, ...) \
			{#name, name}, {#name "-1.0", name},

			XSONCPP_BASE_EXT_SETS
			#undef X


			#define X(name, ...) \
			{#name, name},

			XSONCPP_EXT_SETS
			#undef X
		};
	}
}
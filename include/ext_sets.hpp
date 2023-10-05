#pragma once
#include <bitset>


//This will most likely be moved to a different file in the near future
namespace xson {
	constexpr static std::size_t num_xles = 10 + 1;
}


namespace xson {
	using ext_list = std::bitset<num_xles>;
}


namespace xson {
	namespace impl {
		template<std::size_t... Ids>
		struct ext_set {
			constexpr static ext_list list{ (1 << Ids | ...) };
		};
	}
}


namespace xson {
	namespace ext_set {
		constexpr static ext_list ini  = impl::ext_set<2, 6, 8, 9>::list;
		constexpr static ext_list json = impl::ext_set<3, 5, 6, 7>::list;
		constexpr static ext_list toml = impl::ext_set<2, 3, 6, 8>::list;
	}
}
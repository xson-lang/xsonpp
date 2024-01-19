#pragma once
#include <cstdint>
#include <bitset>

namespace xson {
	namespace ext {
		enum name : std::size_t {
			schema_files,
			omit_kv_separator,
			equals_as_kv_separator,
			all_value_types_in_arrays,
			escaped_chars,
			commas_after_values,
			quoted_strings,
			root_level_object_or_array,
			headers_for_objects,
			alternate_comment_characters,

			num_exts
		};


		constexpr bool supported(std::size_t id) {
			return id < num_exts;
		}
	}
}


namespace xson {
	using ext_list = std::bitset<ext::num_exts>;
}
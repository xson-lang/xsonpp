#pragma once
#include <result.hpp>
#include <result/basic_error_category.hpp>
#include <string_view>
#include <cstdint>

namespace xson::error {
	enum code : std::int_fast32_t {
		unknown,

		//IO errors
		invalid_file,
		cannot_load_file,

		//File parse errors
		duplicate_key,
		duplicate_kv_seperator,
		unmatched_opening_curly_braces,
		unmatched_opening_square_brackets,
		comment_missing_ending,
		unrecognized_directive,
		content_before_directive,
		empty_directive,
		empty_directive_args,
		missing_directive_item,
		missing_last_directive_item,
		unrecognized_ext_set,
		duplicate_hypen,
		missing_ext_id,
		invalid_ext_id,
		invalid_ext_range,
		
		//Key-value parse errors
		key_not_found,

		num_codes,
	};

	//TODO add parameter to descriptions (%s)
	constexpr std::string_view desc[num_codes] = {
		"Duplicate key!",
		"Duplicate key-value seperator!",
		"Missing closing curly brace for unmatched \'{\' character!",
		"Missing closing square bracket for unmatched \'[\' character!",
		"Missing multi-line comment ending (\'*/\')!",
		"Unrecognized directive!",
		"Directive not at top of file! There must not be any key/value pairs before directives.",
		"Empty directive! An unescaped \'#\' character by itself is not allowed: it must contain a directive.",
		"Missing directive arguments! Directive must have a non-empty list!",
		"Missing directive item before comma separator (\',\')! You cannot have a directive list with an empty item!",
		"Missing directive item after last comma separator (\',\')! Directive list cannot end with a comma!",
		"Unrecognized extension set!",
		"Duplicate hypen character (\'-\') in extension id range!",
		"Missing id in extension id range!",
		"Invalid extension id!",
		"Invalid extension id range!",


		"Unexpected/unknown XSON error",
	};
}

OL_RESULT_DECLARE_AS_ERROR_CODE(xson::error, code, &ol::error_category_array<desc>, nullptr, xson)
#pragma once
#include <string_view>
#include <array>
#include <cstdint>

namespace xson::error {
	enum code : std::int_fast32_t {
		none,

		//POSIX errors
		operation_not_permitted,
		no_such_file_or_directory,
		no_such_process,
		interrupted,
		io_error,
		no_such_device_or_address,
		argument_list_too_long,
		executable_format_error,
		bad_file_descriptor,
		no_child_process,
		resouce_unavailable_try_again,
		not_enough_memory,
		permission_denied,

		//XSON errors
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

		num_codes,

		unknown = none,
		posix_range_start = operation_not_permitted,
		posix_range_end = permission_denied,
	};

	//TODO add parameter to descriptions (%s)
	constexpr std::array<std::string_view, num_codes> desc = {
		"Success! (No error)",

		"Operation is not permitted! The process or user may not have the necessary permissions to perform the operation.",
		"File or directory doesn't exist!",
		"", "",
		"Physical read or write error occured!",
		"Could not find the device respresented by the given file!",
		"", "",
		"Bad file descriptor! Cannot read or write a file that isn't open for reading or writing, respectively.",
		"", "",
		"Out of memory! Could not allocate more virtual memory for the operation.",
		"Permission denied! You may need elevated permissions to operate on the given file.",

		"Duplicate key!",
		"Duplicate key-value seperator!"
		"Missing closing curly brace for unmatched \'{\' character!",
		"Missing closing square bracket for unmatched \'[\' character!",
		"Missing multi-line comment ending (\'*/\')!",
		"Unrecognized directive!",
		"Directive not at top of file! There must not be any key/value pairs before directives."
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

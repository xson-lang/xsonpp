#pragma once
#include "XsonCpp/io/parser.hpp"


#include "parse/object.ipp"
#include "parse/directive.ipp"
#include "parse/comment.ipp"

namespace xson {
	error::info parser::make_parse_error(error::code value, std::optional<error_location> l) const noexcept {
		return { value, l.value_or(err_loc).file, l.value_or(err_loc).line, l.value_or(err_loc).col };
	}
}


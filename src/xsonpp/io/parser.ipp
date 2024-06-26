#pragma once
#include "xsonpp/io/parser.hpp"

#include <llfio.hpp>

#include "parser/directive.inl"
#include "parser/comment.inl"

namespace xson {
	error::info parser::make_parse_error(error::code value, std::optional<error_location> l) const noexcept {
		return { value, {l.value_or(err_loc).file, LLFIO_V2_NAMESPACE::path_view_component::zero_terminated}, l.value_or(err_loc).line, l.value_or(err_loc).col };
	}
}


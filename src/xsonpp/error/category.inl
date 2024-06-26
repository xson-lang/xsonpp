#pragma once
#include "xsonpp/error/category.hpp"

namespace xson::error {
	const char* category::name() const noexcept {
		return "XSON Error";
	}


	std::string category::message(int c) const {
		if (c >= 0 && c < num_codes && !desc[c].empty())
			return std::string(desc[c]);

		if (c >= posix_range_start && c <= posix_range_end)
			return std::make_error_code(static_cast<std::errc>(c)).message();

		return std::string(desc[unknown]);
	}

	std::error_condition category::default_error_condition(int c) const noexcept {
		if (c >= posix_range_start && c <= posix_range_end)
			return std::make_error_condition(static_cast<std::errc>(c));

		return std::error_condition(c, *this);
	}
}


namespace xson::error {
	const category& xson_category() noexcept {
		static category c;
		return c;
	}


	std::error_code make_error_code(error::code e)  {
		return { static_cast<int>(e), category() };
	}

	std::error_code make_error_code(error::info e) {
		return make_error_code(e.code());
	}

	std::error_condition make_error_condition(error::code e) {
		return {static_cast<int>(e), category()};
	}

	std::error_condition make_error_condition(error::info e) {
		return make_error_condition(e.code());
	}
}
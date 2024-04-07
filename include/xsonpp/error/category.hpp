#pragma once
#include <system_error>

#include "xsonpp/error/code.hpp"
#include "xsonpp/error/info.hpp"

namespace xson::error {
	struct category : public std::error_category {
		virtual inline const char* name() const noexcept override final;
		virtual inline std::string message(int c) const override final;
		virtual inline std::error_condition default_error_condition(int c) const noexcept override final;
	};
}


namespace xson::error {
	inline const category& xson_category() noexcept;

	inline std::error_code make_error_code(error::code e);
	inline std::error_code make_error_code(error::info e);
}

#include "../src/xsonpp/error/category.inl"
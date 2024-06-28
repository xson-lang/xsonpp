#pragma once
#include <optional>
#include <result.hpp>
#include <outcome/trait.hpp>
#include <llfio.hpp>

#include "xsonpp/error/code.hpp"
#include "xsonpp/error/category.hpp"

#include <outcome/std_result.hpp>


namespace xson::error {
	namespace llfio = LLFIO_V2_NAMESPACE;
	using llfio_err = llfio::error_info;

	struct info {
		constexpr info(); //non-trivial constructor, although a trivial one is possible
		inline info(llfio_err& llfio_error);
		inline info(error::code value, 
			llfio::path_view file_path = {},
			std::optional<std::size_t> file_line = std::nullopt,
			std::optional<std::size_t> file_column = std::nullopt
		);
		inline info(std::errc value, 
			llfio::path_view file_path = {},
			std::optional<std::size_t> file_line = std::nullopt,
			std::optional<std::size_t> file_column = std::nullopt
		);

		constexpr error::code code() const noexcept;
		constexpr explicit operator error::code() const noexcept;

		constexpr const std::error_category& category() const noexcept;
		constexpr llfio::path_view path() const noexcept;
		constexpr std::optional<std::size_t> line() const noexcept;
		constexpr std::optional<std::size_t> column() const noexcept;

		inline std::string location() const;
		inline std::string full_message() const;
		inline std::string message() const;

	private:
		constexpr static std::size_t npos = -1;

		error::code _code;
		const std::error_category* _cat; 
		llfio::path_view _path; //trivial constructor requires to use c-string instead of path_view or the like
		std::size_t _line, _col;
	};
}

OL_RESULT_DECLARE_AS_ERROR_CLASS(xson::error, info, code())

namespace xson {
	template<typename T>
	using result = ol::result<T, xson::error::info>;
}


#include "../src/xsonpp/error/info.inl"
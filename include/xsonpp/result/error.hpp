#pragma once
#include <array>
#include <llfio/v2.0/path_view.hpp>
#include <system_error>
#include <filesystem>
#include <optional>

//required to build shared library: disable use of undefined function that checks filesytem abi
#define LLFIO_DISABLE_INLINE_SIZEOF_FILESYSTEM_PATH_CHECK 1
#include <llfio/v2.0/status_code.hpp>


#include "../export_defs.h"
#include "error_code.hpp"


namespace xson::error {
	namespace llfio = LLFIO_V2_NAMESPACE;
	using llfio_err = llfio::error_info;

	struct info {
		//Same definition always
		constexpr error::code code() const noexcept;
		constexpr operator error::code() const noexcept;

		constexpr const std::error_category& category() const noexcept;
		
		inline std::string message() const;
		
		

		//Different definition if error path is enabled
		inline info(); //non-trivial constructor, although a trivial one is possible
		inline info(llfio_err& llfio_error);
		inline info(error::code value, 
			llfio::path_view file_path = {},
			std::optional<std::size_t> file_line = std::nullopt,
			std::optional<std::size_t> file_column = std::nullopt
		);

		constexpr llfio::path_view path() const noexcept;
		constexpr std::optional<std::size_t> line() const noexcept;
		constexpr std::optional<std::size_t> column() const noexcept;

		inline std::string location() const;
		inline std::string full_message() const;

	private:
		constexpr static std::size_t npos = -1;

		error::code _code;
		const std::error_category* _cat; //info class can be trivially constructed, although that would cause this to be an uninitialized pointer 
		llfio::path_view _path;
		std::size_t _line, _col;
	};
}



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


namespace std {
	template<> struct is_error_code_enum<xson::error::code> : true_type {};
}


#include "../src/error.inl"

#ifdef XSONPP_HEADER_ONLY
#include "../../../src/error.ipp"
#endif
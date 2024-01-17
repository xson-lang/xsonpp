#pragma once
#include <array>
#include <system_error>
#include <filesystem>
#include <optional>

//required to build shared library: disable use of undefined function that checks filesytem abi
#define LLFIO_DISABLE_INLINE_SIZEOF_FILESYSTEM_PATH_CHECK 1

#ifdef XSON_CPP_ERROR_NO_PATH
#define LLFIO_DISABLE_PATHS_IN_FAILURE_INFO true
#define _CONSTEXPR_IF_NO_PATH constexpr
#else
#define _CONSTEXPR_IF_NO_PATH inline
#endif
#include <llfio/v2.0/status_code.hpp>


#include "export_defs.h"
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
		_CONSTEXPR_IF_NO_PATH info(); //non-trivial constructor, although a trivial one is possible
		_CONSTEXPR_IF_NO_PATH info(error::code value);
		inline info(llfio_err& llfio_error);
		inline info(error::code value, std::filesystem::path file_path,
			std::optional<std::size_t> file_line = std::nullopt,
			std::optional<std::size_t> file_column = std::nullopt
		);

		inline std::filesystem::path path() const noexcept;
		constexpr std::optional<std::size_t> line() const noexcept;
		constexpr std::optional<std::size_t> column() const noexcept;

		inline std::string location() const;
		inline std::string full_message() const;

	private:
		constexpr static std::size_t npos = -1;

		error::code _code;
		const std::error_category* _cat; //info class can be trivially constructed, although that would cause this to be an uninitialized pointer 
		#ifndef XSON_CPP_ERROR_NO_PATH
		std::filesystem::path _path;
		std::size_t _line, _col;
		#endif
	};
}

#undef _CONSTEXPR_IF_NO_PATH



namespace xson::error::impl {
	struct category : public std::error_category {
		virtual inline const char* name() const noexcept override final;
		virtual inline std::string message(int c) const override final;
		virtual inline std::error_condition default_error_condition(int c) const noexcept override final;
	};
}


namespace xson::error {
	XSONCPP_EXPORT const impl::category& category() noexcept;

	inline std::error_code make_error_code(error::code e);
	inline std::error_code make_error_code(error::info e);
}


namespace std {
	template<> struct is_error_code_enum<xson::error::code> : true_type {};
}


#include "../src/error.inl"

#ifdef XSON_CPP_HEADER_ONLY
#include "../src/error.ipp"
#endif
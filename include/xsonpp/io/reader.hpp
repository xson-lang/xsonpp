#pragma once
#include <filesystem>
#include <cstdint>

#include <llfio.hpp>
#include <result.hpp>

#include "xsonpp/error/info.hpp"
#include "xsonpp/xson/document.hpp"
#include "xsonpp/export_defs.h"

namespace xson {
	namespace llfio = LLFIO_V2_NAMESPACE;


	struct XSONPP_EXPORT reader {
		constexpr reader() noexcept = default;
		//Destructor calls input file handle destructor, which closes the handle

		result<void> open(std::filesystem::path file_path) noexcept;
		result<void> close() noexcept;

		//template<template<typename...> typename KVMapTy = std::map>
		result<document<>> read();

	private:
		llfio::mapped_file_handle input_handle{};
		std::size_t input_length = 0;
		std::filesystem::path file_loc{};
	};
}

#ifdef XSONPP_HEADER_ONLY
#include "../src/xsonpp/io/reader.ipp"
#endif
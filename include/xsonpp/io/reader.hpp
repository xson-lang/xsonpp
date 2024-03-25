#pragma once
#include <filesystem>
#include <cstdint>

#include <llfio.hpp>

#include "../result/result.hpp"
#include "../xson/document.hpp"

namespace xson {
	namespace llfio = LLFIO_V2_NAMESPACE;


	struct XSONPP_EXPORT reader {
		constexpr reader() noexcept = default;
		//Destructor calls input file handle destructor, which closes the handle

		result<> open(std::filesystem::path file_path) noexcept;
		result<> close() noexcept;

		result<document> read(); //needs testing for inner copy elision?

	private:
		llfio::mapped_file_handle input_handle{};
		std::size_t input_length = 0;
		std::filesystem::path file_loc{};
	};
}

#ifdef XSONPP_HEADER_ONLY
#include "../../../src/reader.ipp"
#endif
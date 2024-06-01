#include <outcome/config.hpp>
#include <outcome/success_failure.hpp>
#include <utility>
#include <zsimd/arch/scalar.hpp>

#include "xsonpp/io/parser.hpp"
#include "xsonpp/io/reader.hpp"
#include "xsonpp/xson/ext_list.hpp"

#define VERIFY_RESULT_UNSCOPED(var, fn) \
auto var = fn; \
if (var.has_error()) \
	return var.assume_error();


namespace xson {
	result<void> reader::open(std::filesystem::path file_path) noexcept {
		VERIFY_RESULT_UNSCOPED(file_result, llfio::mapped_file_handle::mapped_file({}, file_path,
			llfio::mapped_file_handle::mode::read,
			llfio::mapped_file_handle::creation::open_existing,
			llfio::mapped_file_handle::caching::all,
			llfio::mapped_file_handle::flag::none
			//Instead of padded SIMD loading, we could "write" to the memory map with copy-on-write semantics (i.e. MAP_PRIVATE) 
			//Using this we would "append" zeros to the copy-on-write mapping to pad out the input to nearest SIMD vector size
			//(needs testing to see if it actually performs better than a single padded load)
			//llfio::section_handle::flag::cow | llfio::section_handle::flag::read
		));
		input_handle = std::move(file_result).assume_value();

		VERIFY_RESULT_UNSCOPED(file_len, input_handle.maximum_extent());
		input_length = file_len.assume_value();

		file_loc = file_path;
		
		return result<void>{std::in_place_type<void>};
	}

	result<void> reader::close() noexcept {
		if (auto r = input_handle.close(); r.has_error()) \
			return r.assume_error();
		return result<void>{std::in_place_type<void>};
	}

	result<document> reader::read() {
		if (!input_handle.is_valid())
			return error::info{ error::bad_file_descriptor, file_loc};

		if (input_length == 0)
			return document{};
		
		const char* begin = reinterpret_cast<char*>(input_handle.address());
		if (!begin || !(begin + input_length))
			return error::info{ error::resouce_unavailable_try_again, file_loc };

		parser p{ext_list{true << 0}, {1, 1, file_loc}};
		auto r = p.parse<segment::object>(begin, input_length);
		if(!r.has_value())
			return r.error();
		return document{*r};
	}
}


#undef VERIFY_RESULT_UNSCOPED
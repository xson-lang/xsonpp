#include "xsonpp/io/parser.hpp"
#include "xsonpp/io/reader.hpp"
#include "xsonpp/xson/ext_list.hpp"
#include "xsonpp/xson/value/type_traits.hpp"

#include <cstring>
#include <charconv>


#define VERIFY_RESULT_UNSCOPED(var, fn) \
auto var = fn; \
if (var.has_error()) \
	return var.assume_error();


namespace xson {
	result<> reader::open(std::filesystem::path file_path) noexcept {
		VERIFY_RESULT_UNSCOPED(file_result, llfio::mapped_file({}, file_path));
		input_handle = std::move(file_result).assume_value(); //!!!! Needs Testing !!!!

		VERIFY_RESULT_UNSCOPED(file_len, input_handle.maximum_extent());
		input_length = file_len.assume_value();

		file_loc = file_path;

		return impl::success();
	}

	result<> reader::close() noexcept {
		if (auto r = input_handle.close(); r.has_error()) \
			return r.assume_error();
		return impl::success();
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
		if(r.has_error())
			return r.error();
		return document{*r};
	}
}


#undef VERIFY_RESULT_UNSCOPED
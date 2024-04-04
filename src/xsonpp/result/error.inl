#pragma once
#include "xsonpp/result/error.hpp"

#include <future>
#include <llfio/v2.0/path_view.hpp>


namespace xson::error {
	constexpr error::code info::code() const noexcept {
		return _code;
	}

	constexpr info::operator error::code() const noexcept {
		return _code;
	}


	constexpr const std::error_category& info::category() const noexcept {
		if (_cat) return *_cat;
		return xson::error::xson_category();
	}


	inline std::string info::message() const {
		if (_cat) return _cat->message(_code);
		return xson::error::xson_category().message(_code);
	}
}


namespace xson::error {
	info::info() :
		info(error::unknown) {}

	info::info(llfio_err& llfio_error) : 
		_code(static_cast<error::code>(llfio_error.value())), 
		_cat(&llfio::make_error_code(llfio_error).category()),
		_path(llfio_error.path1()), _line(npos), _col(npos) {}

	info::info(error::code value, llfio::path_view file_path,
		std::optional<std::size_t> file_line, std::optional<std::size_t> file_column
	) :
		_code(value), _cat(&error::xson_category()), _path(file_path),
		_line(file_line.value_or(npos)),
		_col(file_column.value_or(npos)) {}


	constexpr llfio::path_view info::path() const noexcept {
		return _path;
	}

	constexpr std::optional<std::size_t> info::line() const noexcept {
		return _line != npos ? std::make_optional(_line) : std::nullopt;
	}

	constexpr std::optional<std::size_t> info::column() const noexcept {
		return _col != npos ? std::make_optional(_col) : std::nullopt;
	}


	std::string info::location() const {
		//Maybe this can be optimized, but it may not be necessary to do so
		auto p = llfio::path_view::rendered_path<llfio::path_view_component::zero_terminated>(_path);
		std::string ret{p.c_str(), p.size()};
		if (ret.empty())
			return "";
		
		if (_line == npos)
			return ret;

		ret.reserve(ret.size() + sizeof('(') + 4 + sizeof(',') + 3 + sizeof(')'));

		ret.append(1, '(').append(std::to_string(_line));
		if (_col != npos)
			ret.append(1, ',').append(std::to_string(_col));
		return ret.append(1, ')');
	}

	std::string info::full_message() const {
		std::string ret = location();
		if (ret.empty())
			return message();

		std::string msg = message();
		ret.reserve(ret.size() + sizeof(':') + sizeof(' ') + msg.size());
		return ret.append(1, ':').append(1, ' ').append(msg);
	}
}



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
}

//namespace xson::error {
//	constexpr bool valid(error::code code) noexcept {
//		return code < num_codes && !desc[code].empty();
//	}
//}
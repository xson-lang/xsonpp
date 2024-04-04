#pragma once
#include <cstdint>
#include <filesystem>

#include <outcome/result.hpp>
#include <outcome/config.hpp>

#include "error.hpp"

namespace xson { struct parser; }

namespace xson::impl {
	namespace outcome = OUTCOME_V2_NAMESPACE;

	//Will probably switch to using terminate, but need to test with -fno-exceptions first
	template<typename T>
	using outcome_result_type = outcome::checked<T, xson::error::info>;
}


namespace xson::impl {
	constexpr outcome::success_type<void> success() noexcept;

	template<typename T>
	constexpr decltype(auto) success(T&& v, uint16_t spare_storage = 0);
}


namespace xson::impl {
	template<typename T>
	struct result_base : protected outcome_result_type<T> {
		using outcome_result_type<T>::outcome_result_type;

		
		using outcome_result_type<T>::operator bool;
		constexpr bool has_success() const noexcept;


		using outcome_result_type<T>::error;
		using outcome_result_type<T>::has_error;


		constexpr error::code code() const noexcept;
		std::string message() const;
		constexpr const std::error_category& category() const noexcept;

		std::filesystem::path path() const noexcept;
		constexpr std::optional<std::size_t> line() const noexcept;
		constexpr std::optional<std::size_t> column() const noexcept;

		std::string location() const;
		std::string full_message() const;

	private:
		friend struct ::xson::parser;
	};
}


namespace xson {
	template<typename T = void> 
	struct [[nodiscard]] result : public impl::result_base<T> {
		using impl::result_base<T>::result_base;

		constexpr const T* operator->() const noexcept;
		constexpr T* operator->() noexcept;

		constexpr const T& operator*() const& noexcept;
		constexpr T& operator*() & noexcept;

		constexpr const T&& operator*() const&& noexcept;
		constexpr T&& operator*() && noexcept;
		

		template<typename U>
		constexpr T value_or(U&& default_value) const&;

		template<typename U>
		constexpr T value_or(U&& default_value) &&;

		using impl::result_base<T>::has_value;
		using impl::result_base<T>::value;
	};

	template<>
	struct [[nodiscard]] result<void> : public impl::result_base<void> {
		using impl::result_base<void>::result_base;
	};
}



#include "../src/xsonpp/result/result.inl"
#pragma once
#include "result.hpp"


namespace xson::impl {
	constexpr outcome::success_type<void> success() noexcept {
		return outcome::success();
	}

	template<typename T>
	constexpr decltype(auto) success(T&& v, uint16_t spare_storage) {
		return outcome::success(std::forward<T>(v), spare_storage);
	}
}


namespace xson::impl {
	template<typename T>
	constexpr bool result_base<T>::has_success() const noexcept {
		return outcome_result_type<T>::has_value();
	}


	template<typename T>
	constexpr error::code result_base<T>::code() const noexcept { 
		return outcome_result_type<T>::assume_error().code();
	}

	template<typename T>
	std::string result_base<T>::message() const {
		return outcome_result_type<T>::assume_error().message();
	}

	template<typename T>
	constexpr const std::error_category& result_base<T>::category() const noexcept {
		return outcome_result_type<T>::assume_error().category();
	}


	template<typename T>
	std::filesystem::path result_base<T>::path() const noexcept {
		return outcome_result_type<T>::assume_error().path();
	}

	template<typename T>
	constexpr std::optional<std::size_t> result_base<T>::line() const noexcept {
		return outcome_result_type<T>::assume_error().line();
	}

	template<typename T>
	constexpr std::optional<std::size_t> result_base<T>::column() const noexcept {
		return outcome_result_type<T>::assume_error().column();
	}


	template<typename T>
	std::string result_base<T>::location() const {
		return outcome_result_type<T>::assume_error().location();
	}

	template<typename T>
	std::string result_base<T>::full_message() const {
		return outcome_result_type<T>::assume_error().full_message();
	}
}


namespace xson {
	template<typename T>
	constexpr const T* result<T>::operator->() const noexcept {
		return std::addressof(impl::result_base<T>::assume_value());
	}

	template<typename T>
	constexpr T* result<T>::operator->() noexcept {
		return std::addressof(impl::result_base<T>::assume_value());
	}


	template<typename T>
	constexpr const T& result<T>::operator*() const& noexcept {
		return impl::result_base<T>::assume_value();
	}

	template<typename T>
	constexpr T& result<T>::operator*() & noexcept {
		return impl::result_base<T>::assume_value();
	}


	template<typename T>
	constexpr const T&& result<T>::operator*() const&& noexcept {
		return std::move(impl::result_base<T>::assume_value());
	}

	template<typename T>
	constexpr T&& result<T>::operator*() && noexcept {
		return std::move(impl::result_base<T>::assume_value());
	}



	template<typename T>
	template<typename U>
	constexpr T result<T>::value_or(U&& default_value) const& {
		return bool(*this) ? **this : static_cast<T>(std::forward<U>(default_value));
	}

	template<typename T>
	template<typename U>
	constexpr T result<T>::value_or(U&& default_value) && {
		return bool(*this) ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
	}
}
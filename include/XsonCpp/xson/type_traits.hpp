#pragma once
#include <uni_str/uni_str.hpp>

#include <type_traits>
#include <cstdint>
#include <string>

namespace xson { struct object; }


namespace xson::impl {
	template<class T>
	struct remove_cvref {
		using type = std::remove_cv_t<std::remove_reference_t<T>>;
	};

	template<class T>
	using remove_cvref_t = typename remove_cvref<T>::type;
}


namespace xson::impl {
	template<class T, typename = void>
	struct has_traits_type : std::false_type {};

	template<class T>
	struct has_traits_type<T, std::void_t<typename T::traits_type>> : std::true_type {};

	template<class T>
	constexpr bool has_traits_type_v = has_traits_type<T>::value;


	template<class T, typename = void>
	struct has_value_type : std::false_type {};

	template<class T>
	struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};

	template<class T>
	constexpr bool has_value_type_v = has_value_type<T>::value;


	template<class T>
	struct has_char_traits : std::is_same<typename T::traits_type, std::char_traits<typename T::value_type>> {};

	template<class T>
	constexpr bool has_char_traits_v = has_char_traits<T>::value;


	template<class T, class... Tys>
	using is_one_of = std::disjunction<std::is_same<T, Tys>...>;

	template<class T, class... Tys>
	constexpr bool is_one_of_v = is_one_of<T, Tys...>::value;
}


namespace xson::impl {
	template<class T>
	using is_string_like = std::disjunction<
		std::conjunction<has_traits_type<T>, has_value_type<T>, has_char_traits<T>>,
		std::is_convertible<ol::string, std::decay_t<T>>,
		std::conjunction<std::is_pointer<std::decay_t<T>>, is_one_of<std::decay_t<T>, char*, wchar_t*, ol::uchar8_t*, char16_t*, char32_t*>>
	>;

	template<class T>
	constexpr bool is_string_like_v = is_string_like<T>::value;
}


namespace xson::impl {
	template<class T, typename = void>
	struct has_subscript_operator : std::false_type {};

	template<class T>
	struct has_subscript_operator<T, std::void_t<decltype(std::declval<T>()[std::size_t{}]) >> : std::true_type {};

	template<class T>
	constexpr bool has_subscript_operator_v = has_subscript_operator<T>::value;
}

namespace xson::impl {
	template<class T>
	using is_array_like = std::conjunction<has_subscript_operator<T>, std::negation<is_string_like<T>>>;

	template<class T>
	constexpr bool is_array_like_v = is_array_like<T>::value;
}


namespace xson::impl {
	template<class T>
	using is_not_specialized = std::conjunction<
		std::negation<is_array_like<T>>, 
		std::negation<std::is_arithmetic<remove_cvref_t<T>>>,
		std::negation<std::is_convertible<T, xson::object>>
	>;

	template<class T>
	constexpr bool is_not_specialized_v = is_not_specialized<T>::value;
}
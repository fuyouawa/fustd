#pragma once
#include <type_traits>
#include <tuple>
#include <concepts>

#define ALL_INTEGRAL_TYPES bool, char, signed char, unsigned char, wchar_t, char16_t, char32_t, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long

#define ALL_FLOATING_POINT_TYPES float, double, long double

namespace fustd {
namespace details {
template<typename T>
struct nude {
	using type = std::decay_t<T>;
};

template<typename T>
struct nude<T[]> {
	using type = typename nude<T>::type;
};

template<typename T>
struct nude<T*> {
	using type = typename nude<T>::type;
};

template <template <typename...> typename T>
struct TemplateType {};

template <typename T>
struct ExtractTemplate {
	static constexpr bool kIsTemplate = false;
};

template <template <typename...> typename T, typename... ArgsType>
struct ExtractTemplate<T<ArgsType...>> {
	static constexpr bool kIsTemplate = true;
	using Type = TemplateType<T>;
};

template <typename T, typename U>
constexpr bool EqualTemplate() {
	if constexpr (ExtractTemplate<T>::kIsTemplate != ExtractTemplate<U>::kIsTemplate) {
		return false;
	}
	else {
		return std::is_same_v<typename ExtractTemplate<T>::Type, typename ExtractTemplate<U>::Type>;
	}
}
}

template <typename T, typename U>
constexpr bool kIsSameTemplate = details::EqualTemplate<T, U>();
}
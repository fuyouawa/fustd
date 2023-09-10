#pragma once
#include <stdint.h>
#include <fustd/generic/type_traits.hpp>

namespace fustd {
template<class T, class E, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<E>, int> = 0>
constexpr T FloorDiv(T dividend, E divisor) {
	return dividend / divisor;
}

template<class T, class E, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<E>, int> = 0>
constexpr T CeilDiv(T dividend, E divisor) {
	T res = dividend / divisor;
	return (dividend % divisor) != 0 ? res + 1 : res;
}
}
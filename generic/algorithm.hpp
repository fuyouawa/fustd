#pragma once
#include <stdint.h>
#include <fustd/generic/type_traits.hpp>

namespace fustd {
template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
constexpr T FloorDiv(T dividend, T divisor) {
	return dividend / divisor;
}

template<class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
constexpr T CeilDiv(T dividend, T divisor) {
	T res = dividend / divisor;
	return (dividend % divisor) != 0 ? res + 1 : res;
}
}
#pragma once
#include <stdint.h>
#include <fustd/generic/type_traits.hpp>

namespace fustd {
namespace details {
template<size_t cur_va_idx, class TraverseCallbackT>
inline void TraverseVariable(const TraverseCallbackT& callback, size_t begin_idx, size_t count) {}

template<size_t cur_va_idx, class TraverseCallbackT, class ThisType, class... RestTypes>
inline void TraverseVariable(const TraverseCallbackT& callback, size_t begin_idx, size_t count, ThisType&& this_type, RestTypes&&... rest_type) {
	if (cur_va_idx > begin_idx + count)
		return;
	if (cur_va_idx >= begin_idx)
		callback(std::forward<ThisType>(this_type), cur_va_idx);
	TraverseVariable<cur_va_idx + 1, TraverseCallbackT, RestTypes...>(callback, begin_idx, count, std::forward<RestTypes>(rest_type)...);
}
}

template<class T, class E, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<E>, int> = 0>
constexpr T FloorDiv(T dividend, E divisor) {
	return dividend / divisor;
}

template<class T, class E, std::enable_if_t<std::is_integral_v<T> && std::is_integral_v<E>, int> = 0>
constexpr T CeilDiv(T dividend, E divisor) {
	T res = dividend / divisor;
	return (dividend % divisor) != 0 ? res + 1 : res;
}

template<class TraverseCallbackT, class... Types>
inline void TraverseVariable(const TraverseCallbackT& callback, size_t begin_idx, size_t count, Types&&... types)
{
	details::TraverseVariable<0, TraverseCallbackT, Types...>(
		callback,
		begin_idx,
		count,
		std::forward<Types>(types)...);
}
}
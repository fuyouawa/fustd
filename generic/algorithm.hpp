#pragma once
#include <stdint.h>
#include <fustd/generic/type_traits.hpp>

namespace fustd {
namespace details {
template<size_t cur_va_idx, class TraverseCallbackT>
inline void TraverseVariable(const TraverseCallbackT& callback, size_t begin_idx, size_t count) {}

template<size_t cur_va_idx, class TraverseCallbackT, class ThisType, class... RestTypes>
inline void TraverseVariable(const TraverseCallbackT& callback, size_t begin_idx, size_t count, ThisType&& this_type, RestTypes&&... rest_type) {
	if (cur_va_idx >= begin_idx + count)
		return;
	if (cur_va_idx >= begin_idx)
		callback(std::forward<ThisType>(this_type), cur_va_idx);
	TraverseVariable<cur_va_idx + 1, TraverseCallbackT, RestTypes...>(callback, begin_idx, count, std::forward<RestTypes>(rest_type)...);
}

template<size_t Bytes>
constexpr int AlignBytes() {
	static_assert(Bytes < 8, "Bytes must be less than 8!");
	static_assert(Bytes != 0, "Bytes must be greater than 0!");
	if constexpr (Bytes <= 2)
		return Bytes;
	else if constexpr (Bytes <= 4)
		return 4;
	else return 8;
}
template<bool IsUnsigned,size_t Bytes>
struct BytesToType;
template<> struct BytesToType<true,  1>	{ using Type = uint8_t;	 };
template<> struct BytesToType<false, 1> { using Type = int8_t;	 };
template<> struct BytesToType<true,  2>	{ using Type = uint16_t; };
template<> struct BytesToType<false, 2> { using Type = int16_t;	 };
template<> struct BytesToType<true,  4>	{ using Type = uint32_t; };
template<> struct BytesToType<false, 4> { using Type = int32_t;	 };
template<> struct BytesToType<true,  8>	{ using Type = uint64_t; };
template<> struct BytesToType<false, 8> { using Type = int64_t;	 };
template<size_t Bytes, class T>
using BytesToTypeT = typename details::BytesToType<std::is_unsigned_v<T>, details::AlignBytes<Bytes>()>::Type;
}

template<size_t Bytes, integeral_t T>
	requires (Bytes <= sizeof(T))
void SetIntegerHigh(T& target, details::BytesToTypeT<Bytes, T> value) {
	auto dest = (char*)&target + (sizeof(T) - Bytes);
	memcpy(dest, &value, Bytes);
}


template<integeral_t T, integeral_t E>
constexpr T FloorDiv(T dividend, E divisor) {
	return dividend / divisor;
}

template<integeral_t T, integeral_t E>
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
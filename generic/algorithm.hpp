#pragma once
#include <stdint.h>
#include <fustd/generic/type_traits.hpp>
#include <fustd/generic/details/def.hpp>

FUSTD_BEGIN_NAMESPACE

namespace details {
template<size_t cur_va_idx, class TraverseCallback>
inline void TraverseVariable(const TraverseCallback& callback, size_t begin_idx, size_t count) {}

template<size_t cur_va_idx, class TraverseCallback, class ThisType, class... RestTypes>
inline void TraverseVariable(const TraverseCallback& callback, size_t begin_idx, size_t count, ThisType&& this_type, RestTypes&&... rest_type) {
	if (cur_va_idx >= begin_idx + count)
		return;
	if (cur_va_idx >= begin_idx)
		callback(std::forward<ThisType>(this_type), cur_va_idx);
	TraverseVariable<cur_va_idx + 1, TraverseCallback, RestTypes...>(callback, begin_idx, count, std::forward<RestTypes>(rest_type)...);
}

template<size_t bytes>
constexpr int AlignBytes() {
	static_assert(bytes < 8, "bytes must be less than 8!");
	static_assert(bytes != 0, "bytes must be greater than 0!");
	if constexpr (bytes <= 2)
		return bytes;
	else if constexpr (bytes <= 4)
		return 4;
	else return 8;
}
template<bool is_unsigned, size_t bytes>
struct BytesToType;
template<> struct BytesToType<true,  1>	{ using Type = uint8_t;	 };
template<> struct BytesToType<false, 1> { using Type = int8_t;	 };
template<> struct BytesToType<true,  2>	{ using Type = uint16_t; };
template<> struct BytesToType<false, 2> { using Type = int16_t;	 };
template<> struct BytesToType<true,  4>	{ using Type = uint32_t; };
template<> struct BytesToType<false, 4> { using Type = int32_t;	 };
template<> struct BytesToType<true,  8>	{ using Type = uint64_t; };
template<> struct BytesToType<false, 8> { using Type = int64_t;	 };
template<size_t bytes, class T>
using BytesToTypeT = typename BytesToType<std::is_unsigned_v<T>, AlignBytes<bytes>()>::Type;
}

template<size_t bytes, std::integral T>
	requires (bytes <= sizeof(T))
void SetIntegerHigh(T& target, details::BytesToTypeT<bytes, T> value) {
	auto dest = (char*)&target + (sizeof(T) - bytes);
	memcpy(dest, &value, bytes);
}


template<std::integral T, std::integral E>
constexpr T FloorDiv(T dividend, E divisor) {
	return dividend / divisor;
}

template<std::integral T, std::integral E>
constexpr T CeilDiv(T dividend, E divisor) {
	T res = dividend / divisor;
	return (dividend % divisor) != 0 ? res + 1 : res;
}

template<class TraverseCallback, class... Types>
inline void TraverseVariable(const TraverseCallback& callback, size_t begin_idx, size_t count, Types&&... types)
{
	details::TraverseVariable<0, TraverseCallback, Types...>(
		callback,
		begin_idx,
		count,
		std::forward<Types>(types)...);
}

FUSTD_END_NAMESPACE
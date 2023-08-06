#pragma once
#include <type_traits>

namespace fustd {
template <template <typename...> class Trait, typename Enabler, typename... Args>
struct is_detected : std::false_type {};

template <template <typename...> class Trait, typename... Args>
struct is_detected<Trait, std::void_t<Trait<Args...>>, Args...> : std::true_type {};

template <template <typename...> class Trait, typename... Args>
inline constexpr bool is_detected_v = is_detected<Trait, void, Args...>::value;

template <typename T>
using equality_test = decltype(std::declval<T>() == std::declval<T>());

template <typename T>
inline constexpr bool has_equality_operator_v = is_detected_v<equality_test, T>;
}
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

// 主模板
template <typename T>
struct conditional_rvalue_ref {
    using type = typename std::conditional<std::is_class<T>::value, T&&, T>::type;
};

// 用于简化取类型的别名
template <typename T>
using conditional_rvalue_ref_t = typename conditional_rvalue_ref<T>::type;
}
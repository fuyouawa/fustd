#pragma once
#include <tuple>
#include <concepts>
#include <cassert>

namespace fustd {
template<typename Func>
struct FunctionTraits;

template<typename Ret, typename... Args>
struct FunctionTraits<Ret(Args...)> {
    using Return = Ret;
    using Arguments = std::tuple<Args...>;
    using Function = Ret(*)(Args...);

    static constexpr size_t kArgumentCount = sizeof...(Args);
    static constexpr bool kIsMemberFunction = false;
    static constexpr bool kIsPointerToFunction = false;
    static constexpr bool kIsInvokeableObject = false;
};


template<typename Ret, typename... Args>
struct FunctionTraits<Ret(*)(Args...)> {
    using Return = Ret;
    using Arguments = std::tuple<Args...>;
    using Function = Ret(*)(Args...);

    static constexpr size_t kArgumentCount = sizeof...(Args);
    static constexpr bool kIsMemberFunction = false;
    static constexpr bool kIsPointerToFunction = true;
    static constexpr bool kIsInvokeableObject = false;
};

template<typename Ret, typename Obj, typename... Args>
struct FunctionTraits<Ret(Obj::*)(Args...)> {
    using Return = Ret;
    using Arguments = std::tuple<Args...>;
    using Function = Ret(Obj::*)(Args...);
    using Object = Obj;

    static constexpr size_t kArgumentCount = sizeof...(Args);
    static constexpr bool kIsMemberFunction = true;
    static constexpr bool kIsPointerToFunction = true;
    static constexpr bool kIsInvokeableObject = false;
};

template<typename Ret, typename Obj, typename... Args>
struct FunctionTraits<Ret(Obj::*)(Args...) const> {
    using Return = Ret;
    using Arguments = std::tuple<Args...>;
    using Function = Ret(Obj::*)(Args...) const;
    using Object = Obj;

    static constexpr size_t kArgumentCount = sizeof...(Args);
    static constexpr bool kIsMemberFunction = true;
    static constexpr bool kIsPointerToFunction = true;
    static constexpr bool kIsInvokeableObject = false;
};

template<typename Func>
struct FunctionTraits {
private:
    using Wrapper = FunctionTraits<decltype(&Func::operator())>;
public:
    using Return = typename Wrapper::Return;
    using Arguments = typename Wrapper::Arguments;
    using Function = typename Wrapper::Function;

    static constexpr size_t kArgumentCount = Wrapper::kArgumentCount;
    static constexpr bool kIsMemberFunction = Wrapper::kIsMemberFunction;
    static constexpr bool kIsPointerToFunction = Wrapper::kIsPointerToFunction;
    static constexpr bool kIsInvokeableObject = true;
};

template<typename Func>
struct FunctionTraits<Func&> : public FunctionTraits<Func> {};

template<typename Func>
struct FunctionTraits<Func&&> : public FunctionTraits<Func> {};

template<typename... ExpectedArgs, typename... ActualArgs>
consteval bool MatchArguments(std::tuple<ExpectedArgs...>, std::tuple<ActualArgs...>) {
    return (... && std::is_convertible_v<ActualArgs, ExpectedArgs>);
}

template<typename ExpectedFunc, typename ActualFunc>
consteval bool MatchFunctionArguments() {
    using ExpectedSignature = FunctionTraits<ExpectedFunc>;
    using ActualSignature = FunctionTraits<ActualFunc>;

    using ExpectedArgs = typename ExpectedSignature::Arguments;
    using ActualArgs = typename ActualSignature::Arguments;

    if constexpr (ExpectedSignature::kArgumentCount == ActualSignature::kArgumentCount) {
        return MatchArguments(ExpectedArgs{}, ActualArgs{});
    }
    return false;
}

// ƥ�������ɵ��ö���Ĳ���
// Ҳ����ActualFunc��ÿ�������Ƿ񶼿�����ʽת��ΪExpectedFunc��Ӧ�Ĳ���
template<typename ExpectedFunc, typename ActualFunc>
constexpr bool kIsArgumentsMatchableFunctions = MatchFunctionArguments<ExpectedFunc, ActualFunc>();

// ƥ�������ɵ��ö���ķ���ֵ
// Ҳ����ActualFunc�ķ��������Ƿ������ʽת��ΪExpectedFunc�ķ�������
template<typename ExpectedFunc, typename ActualFunc>
constexpr bool kIsReturnTypeMatchableFunctions = std::is_convertible_v<typename FunctionTraits<ExpectedFunc>::Return,
    typename FunctionTraits<ActualFunc>::Return>;

// ƥ�������ɵ��ö���
template<typename ExpectedFunc, typename ActualFunc>
constexpr bool kIsMatchableFunctions = kIsArgumentsMatchableFunctions<ExpectedFunc, ActualFunc>&&
    kIsReturnTypeMatchableFunctions< ExpectedFunc, ActualFunc>;


namespace internal {
template<typename, typename, typename, typename>
struct ApplyRangeHelper;

template<size_t... II, typename... Args, typename Func, typename ArgsRange>
struct ApplyRangeHelper<std::index_sequence<II...>, std::tuple<Args...>, Func, ArgsRange>
{
    auto operator()(Func&& func, ArgsRange&& args) noexcept(std::is_nothrow_invocable_v<Func, Args...>)
    {
        static_assert(sizeof...(II) == sizeof...(Args));
        return std::invoke(std::forward<Func>(func), (*reinterpret_cast<Args*>(args[II]))...);
    }
};
}   // namespace internal

// ������Ĳ�������չ��, ���Ҷ�Ӧת��ΪArgs����
// ����:
//  func: ���ܲ���չ���Ļص�����
//  args: �洢����������, �����ǿ�������ʵ�
template<typename... Args, typename Func, std::ranges::random_access_range ArgsRange>
auto ApplyRange(Func&& func, ArgsRange&& args) noexcept(std::is_nothrow_invocable_v<Func, Args...>)
{
    assert(sizeof...(Args) == std::size(args));
    return internal::ApplyRangeHelper<
        std::make_index_sequence<sizeof...(Args)>,
        std::tuple<Args...>,
        Func,
        ArgsRange>{}(std::forward<Func>(func), std::forward<ArgsRange>(args));
}
}
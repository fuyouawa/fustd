#pragma once
#include <tuple>
#include <ranges>
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

// ��������ƥ��ģʽ
enum class MatchModel
{
    kConvertible,       // ÿ�����������Զ�Ӧ��ʽת��
    kSame,              // ÿ������������ͬ
    kConvertibleSafety  // ��������ǻ�������(integer, floating)����, ��ô������ʿת��; ���������ͬ
};
namespace internal {
template<typename T>
constexpr bool kIsNumber = std::is_integral_v<T> || std::is_floating_point_v<T>;
template<typename T>
constexpr bool kAlwaysFalse = false;

template<typename, typename, MatchModel>
struct MatchFunctionArgumentsHelper;

template<typename... ExpectArgs, typename... ActualArgs, MatchModel model>
struct MatchFunctionArgumentsHelper<std::tuple<ExpectArgs...>, std::tuple<ActualArgs...>, model>
{
    consteval bool operator()() const {
        if constexpr (sizeof...(ExpectArgs) == sizeof...(ActualArgs)) {
            switch (model)
            {
            case MatchModel::kConvertible:
                return (... && std::is_convertible_v<ActualArgs, ExpectArgs>);
            case MatchModel::kSame:
                return (... && std::is_same_v<ActualArgs, ExpectArgs>);
            case MatchModel::kConvertibleSafety:
                return (... && (kIsNumber<ExpectArgs> ? std::is_convertible_v<ActualArgs, ExpectArgs> :
                    std::is_same_v<ActualArgs, ExpectArgs>));
            default:
                break;
            }
        }
        return false;
    }
};
template<typename ExpectedFunc, typename ActualFunc, MatchModel model>
consteval bool MatchFunctionArguments() {
    using ExpectedArguments = typename FunctionTraits<ExpectedFunc>::Arguments;
    using ActualArguments = typename FunctionTraits<ActualFunc>::Arguments;
    return internal::MatchFunctionArgumentsHelper<ExpectedArguments, ActualArguments, model>{}();
}


template<typename ExpectedFunc, typename ActualFunc, MatchModel model>
consteval bool MatchFunctionReturn() {
    using ExpectedReturn = typename FunctionTraits<ExpectedFunc>::Return;
    using ActualReturn = typename FunctionTraits<ActualFunc>::Return;
    switch (model)
    {
    case MatchModel::kConvertible:
        return std::is_convertible_v<ActualReturn, ExpectedReturn>;
    case MatchModel::kSame:
        return std::is_same_v<ActualReturn, ExpectedReturn>;
    case MatchModel::kConvertibleSafety:
        return kIsNumber<ExpectedReturn> ? std::is_convertible_v<ActualReturn, ExpectedReturn> :
                                            std::is_same_v<ActualReturn, ExpectedReturn>;
    default:
        break;
    }
    return false;
}
}

// ƥ�������ɵ��ö���Ĳ���
// Tips:
// ����ExpectedFunc = void(int, float, MyClass1)
//     ActualFunc = void(double, size_t, MyClass2)
// ��ô����int��doubleƥ��, float��size_tƥ��, MyClass1��MyClass2ƥ��
// ƥ�䷽ʽ��model����
// PS: ƥ�䷽��ΪFrom: ActualFunc; To: ExpectFunc
template<typename ExpectedFunc, typename ActualFunc, MatchModel model = MatchModel::kConvertibleSafety>
constexpr bool kIsArgumentsMatchableFunction = internal::MatchFunctionArguments<ExpectedFunc, ActualFunc, model>();

// ƥ�������ɵ��ö���ķ���ֵ
// Tips:
// ����ExpectedFunc = int()
//     ActualFunc = float()
// ��ô����int��floatƥ��
// ƥ�䷽ʽ��model����
// PS: ƥ�䷽��ΪFrom: ActualFunc; To: ExpectFunc
template<typename ExpectedFunc, typename ActualFunc, MatchModel model = MatchModel::kConvertibleSafety>
constexpr bool kIsReturnTypeMatchableFunction = internal::MatchFunctionReturn<ExpectedFunc, ActualFunc, model>();

// ƥ�������ɵ��ö���
// Tips:
// ����鿴kIsArgumentsMatchableFunction��kIsReturnTypeMatchableFunction��ע��
// ���ֶξ����������ֶν����&&
template<typename ExpectedFunc, typename ActualFunc, MatchModel model = MatchModel::kConvertibleSafety>
constexpr bool kIsMatchableFunction = kIsArgumentsMatchableFunction<ExpectedFunc, ActualFunc, model> &&
                                      kIsReturnTypeMatchableFunction< ExpectedFunc, ActualFunc, model>;


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
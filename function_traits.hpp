#pragma once
#include <tuple>

namespace fustd {
template <typename, typename, typename, typename>
struct FunctorCall;

template <int... II, typename... Args, typename Ret, typename Function>
struct FunctorCall<std::index_sequence<II...>, std::tuple<Args...>, Ret, Function> {
    static Ret Call(Function& func, void** arg) {
        return func(*reinterpret_cast<typename std::remove_reference_t<Args>*>(arg[II])...);
    }
};

template <int... II, typename... Args, typename Ret, typename... Args2, typename Ret2, typename Obj>
struct FunctorCall<std::index_sequence<II...>, std::tuple<Args...>, Ret, Ret2(Obj::*)(Args2...)> {
    static Ret2 Call(Ret2(Obj::* func)(Args2...), Obj* obj, void** arg) {
        return (obj->*func)(*reinterpret_cast<typename std::remove_reference_t<Args>*>(arg[II])...);
    }
};

template <int... II, typename... Args, typename Ret, typename... Args2, typename Ret2, typename Obj>
struct FunctorCall<std::index_sequence<II...>, std::tuple<Args...>, Ret, Ret2(Obj::*)(Args2...) const> {
    static Ret2 Call(Ret2(Obj::* func)(Args2...) const, Obj* obj, void** arg) {
        return (obj->*func)(*reinterpret_cast<typename std::remove_reference_t<Args>*>(arg[II])...);
    }
};

template<typename FuncType>
struct FunctionTraits;

template<typename Ret, typename... Args>
struct FunctionTraits<Ret(Args...)> {
    using Return = Ret;
    using Arguments = std::tuple<Args...>;
    using Function = Ret(*)(Args...);

    static constexpr size_t kArgumentCount = sizeof...(Args);
    static constexpr bool kIsMemberFunction = false;
    static constexpr bool kIsPointerToFunction = false;
    static constexpr bool kIsCallableObject = false;

    template<typename Args2, typename Ret2>
    static Ret2 Call(Function func, void** args) {
        return FunctorCall<typename std::make_index_sequence<kArgumentCount>, Args2, Ret2, Function>::Call(func, args);
    }
};


template<typename Ret, typename... Args>
struct FunctionTraits<Ret(*)(Args...)> {
    using Return = Ret;
    using Arguments = std::tuple<Args...>;
    using Function = Ret(*)(Args...);

    static constexpr size_t kArgumentCount = sizeof...(Args);
    static constexpr bool kIsMemberFunction = false;
    static constexpr bool kIsPointerToFunction = true;
    static constexpr bool kIsCallableObject = false;

    template<typename Args2, typename Ret2>
    static Ret2 Call(Function func, void** args) {
        return FunctorCall<typename std::make_index_sequence<kArgumentCount>, Args2, Ret2, Function>::Call(func, args);
    }
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
    static constexpr bool kIsCallableObject = false;

    template<typename Args2, typename Ret2>
    static Ret2 Call(Function func, Obj* obj, void** args) {
        return FunctorCall<typename std::make_index_sequence<kArgumentCount>, Args2, Ret2, Function>::Call(func, obj, args);
    }
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
    static constexpr bool kIsCallableObject = false;

    template<typename Args2, typename Ret2>
    static Ret2 Call(Function func, Obj* obj, void** args) {
        return FunctorCall<typename std::make_index_sequence<kArgumentCount>, Args2, Ret2, Function>::Call(func, obj, args);
    }
};

template<typename FuncType>
struct FunctionTraits {
private:
    using Wrapper = FunctionTraits<decltype(&FuncType::operator())>;
public:
    using Return = typename Wrapper::Return;
    using Arguments = typename Wrapper::Arguments;
    using Function = typename Wrapper::Function;

    static constexpr size_t kArgumentCount = Wrapper::kArgumentCount;
    static constexpr bool kIsMemberFunction = Wrapper::kIsMemberFunction;
    static constexpr bool kIsPointerToFunction = Wrapper::kIsPointerToFunction;
    static constexpr bool kIsCallableObject = true;

    template<typename Args2, typename Ret2>
    static Ret2 Call(Function func, void** args) {
        return FunctorCall<typename std::make_index_sequence<kArgumentCount>, Args2, Ret2, Function>::Call(func, args);
    }
};

template<typename FuncType>
struct FunctionTraits<FuncType&> : public FunctionTraits<FuncType> {};

template<typename FuncType>
struct FunctionTraits<FuncType&&> : public FunctionTraits<FuncType> {};

template<typename... FuncTypes>
inline constexpr bool EqualFunctionsArguments() {
	static_assert(sizeof...(FuncTypes) >= 2, "The number of comparison function types must be greater than or equal to 2!");
	using FirstFuncArgs = typename FunctionTraits<std::tuple_element_t<0, std::tuple<FuncTypes...>>>::Arguments;
	return (std::is_same_v<FirstFuncArgs, typename FunctionTraits<FuncTypes>::Arguments> && ...);
}

template<typename... FuncTypes>
inline constexpr bool EqualFunctionsReturnType() {
	static_assert(sizeof...(FuncTypes) >= 2, "The number of comparison function types must be greater than or equal to 2!");
	using FirstFuncRetT = typename FunctionTraits<std::tuple_element_t<0, std::tuple<FuncTypes...>>>::Return;
	return (std::is_same_v<FirstFuncRetT, typename FunctionTraits<FuncTypes>::Return> && ...);
}

template<typename... FuncTypes>
inline constexpr bool is_all_same_args_func_v = EqualFunctionsArguments<FuncTypes...>();

template<typename... FuncTypes>
inline constexpr bool is_all_same_rett_func_v = EqualFunctionsReturnType<FuncTypes...>();
}
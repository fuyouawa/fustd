#pragma once
#include <tuple>

namespace fustd {
template<class FuncT>
struct FunctionTraits;

template<class RetT, class... ArgsT>
struct FunctionTraits<RetT(ArgsT...)> {
	using ReturnT = RetT;
	using Arguments = std::tuple<ArgsT...>;
	using FunctionT = RetT(*)(ArgsT...);

	static constexpr size_t kArgumentCount = sizeof...(ArgsT);
	static constexpr bool kIsMemberFunction = false;
	static constexpr bool kIsPointerToFunction = false;
	static constexpr bool kIsCallableObject = false;
};


template<class RetT, class... ArgsT>
struct FunctionTraits<RetT(*)(ArgsT...)> {
	using ReturnT = RetT;
	using Arguments = std::tuple<ArgsT...>;
	using FunctionT = RetT(*)(ArgsT...);

	static constexpr size_t kArgumentCount = sizeof...(ArgsT);
	static constexpr bool kIsMemberFunction = false;
	static constexpr bool kIsPointerToFunction = true;
	static constexpr bool kIsCallableObject = false;
};

template<class RetT, class ObjT, class... ArgsT>
struct FunctionTraits<RetT(ObjT::*)(ArgsT...)> {
	using ReturnT = RetT;
	using Arguments = std::tuple<ArgsT...>;
	using FunctionT = RetT(ObjT::*)(ArgsT...);
	using ObjectT = ObjT;

	static constexpr size_t kArgumentCount = sizeof...(ArgsT);
	static constexpr bool kIsMemberFunction = true;
	static constexpr bool kIsPointerToFunction = true;
	static constexpr bool kIsCallableObject = false;
};

template<class RetT, class ObjT, class... ArgsT>
struct FunctionTraits<RetT(ObjT::*)(ArgsT...) const> {
	using ReturnT = RetT;
	using Arguments = std::tuple<ArgsT...>;
	using FunctionT = RetT(ObjT::*)(ArgsT...) const;
	using ObjectT = ObjT;

	static constexpr size_t kArgumentCount = sizeof...(ArgsT);
	static constexpr bool kIsMemberFunction = true;
	static constexpr bool kIsPointerToFunction = true;
	static constexpr bool kIsCallableObject = false;
};

template<class FuncT>
struct FunctionTraits {
private:
	using Wrapper = FunctionTraits<decltype(&FuncT::operator())>;
public:
	using ReturnT = typename Wrapper::ReturnT;
	using Arguments = typename Wrapper::Arguments;
	using FunctionT = typename Wrapper::FunctionT;

	static constexpr size_t kArgumentCount = Wrapper::kArgumentCount;
	static constexpr bool kIsMemberFunction = Wrapper::kIsMemberFunction;
	static constexpr bool kIsPointerToFunction = Wrapper::kIsPointerToFunction;
	static constexpr bool kIsCallableObject = true;
};

template<class FuncT>
struct FunctionTraits<FuncT&> : public FunctionTraits<FuncT> {};

template<class FuncT>
struct FunctionTraits<FuncT&&> : public FunctionTraits<FuncT> {};

template<class... FuncTypes>
inline constexpr bool EqualFunctionsArguments() {
	static_assert(sizeof...(FuncTypes) >= 2, "The number of comparison function types must be greater than or equal to 2!");
	using FirstFuncArgs = typename FunctionTraits<std::tuple_element_t<0, std::tuple<FuncTypes...>>>::Arguments;
	return (std::is_same_v<FirstFuncArgs, typename FunctionTraits<FuncTypes>::Arguments> && ...);
}

template<class... FuncTypes>
inline constexpr bool EqualFunctionsReturnType() {
	static_assert(sizeof...(FuncTypes) >= 2, "The number of comparison function types must be greater than or equal to 2!");
	using FirstFuncRetT = typename FunctionTraits<std::tuple_element_t<0, std::tuple<FuncTypes...>>>::ReturnT;
	return (std::is_same_v<FirstFuncRetT, typename FunctionTraits<FuncTypes>::ReturnT> && ...);
}

template<class... FuncTypes>
inline constexpr bool is_all_same_args_func_v = EqualFunctionsArguments<FuncTypes...>();

template<class... FuncTypes>
inline constexpr bool is_all_same_rett_func_v = EqualFunctionsReturnType<FuncTypes...>();
}
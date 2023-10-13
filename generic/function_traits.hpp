#pragma once
#include <tuple>
#include "fustd/generic/algorithm.hpp"

FUSTD_BEGIN_NAMESPACE

template<class FuncType>
struct FunctionTraits;

template<class RetType, class... ArgsType>
struct FunctionTraits<RetType(ArgsType...)> {
	using Return = RetType;
	using Arguments = std::tuple<ArgsType...>;
	using Function = RetType(*)(ArgsType...);

	static constexpr size_t kArgumentCount = sizeof...(ArgsType);
	static constexpr bool kIsMemberFunction = false;
	static constexpr bool kIsPointerToFunction = false;
	static constexpr bool kIsCallableObject = false;
};


template<class RetType, class... ArgsType>
struct FunctionTraits<RetType(*)(ArgsType...)> {
	using Return = RetType;
	using Arguments = std::tuple<ArgsType...>;
	using Function = RetType(*)(ArgsType...);

	static constexpr size_t kArgumentCount = sizeof...(ArgsType);
	static constexpr bool kIsMemberFunction = false;
	static constexpr bool kIsPointerToFunction = true;
	static constexpr bool kIsCallableObject = false;
};

template<class RetType, class ObjType, class... ArgsType>
struct FunctionTraits<RetType(ObjType::*)(ArgsType...)> {
	using Return = RetType;
	using Arguments = std::tuple<ArgsType...>;
	using Function = RetType(ObjType::*)(ArgsType...);
	using ObjectT = ObjType;

	static constexpr size_t kArgumentCount = sizeof...(ArgsType);
	static constexpr bool kIsMemberFunction = true;
	static constexpr bool kIsPointerToFunction = true;
	static constexpr bool kIsCallableObject = false;
};

template<class FuncType>
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
};

template<class RetType, class ObjType, class... ArgsType>
struct FunctionTraits<RetType(ObjType::*)(ArgsType...) const> {
	using Return = RetType;
	using Arguments = std::tuple<ArgsType...>;
	using Function = RetType(ObjType::*)(ArgsType...) const;
	using ObjectT = ObjType;

	static constexpr size_t kArgumentCount = sizeof...(ArgsType);
	static constexpr bool kIsMemberFunction = true;
	static constexpr bool kIsPointerToFunction = true;
	static constexpr bool kIsCallableObject = false;
};

template<class FuncType>
struct FunctionTraits<FuncType&> : public FunctionTraits<FuncType> {};

template<class FuncType>
struct FunctionTraits<FuncType&&> : public FunctionTraits<FuncType> {};

template<class... FuncTypes>
inline constexpr bool EqualFunctionsArguments() {
	static_assert(sizeof...(FuncTypes) >= 2, "The number of comparison function types must be greater than or equal to 2!");
	using FirstFuncArgs = typename FunctionTraits<std::tuple_element_t<0, std::tuple<FuncTypes...>>>::Arguments;
	return (std::is_same_v<FirstFuncArgs, typename FunctionTraits<FuncTypes>::Arguments> && ...);
}

template<class... FuncTypes>
inline constexpr bool EqualFunctionsReturnType() {
	static_assert(sizeof...(FuncTypes) >= 2, "The number of comparison function types must be greater than or equal to 2!");
	using FirstFuncRetT = typename FunctionTraits<std::tuple_element_t<0, std::tuple<FuncTypes...>>>::Return;
	return (std::is_same_v<FirstFuncRetT, typename FunctionTraits<FuncTypes>::Return> && ...);
}

template<class... FuncTypes>
inline constexpr bool is_all_same_args_func_v = EqualFunctionsArguments<FuncTypes...>();

template<class... FuncTypes>
inline constexpr bool is_all_same_rett_func_v = EqualFunctionsReturnType<FuncTypes...>();

FUSTD_END_NAMESPACE
#pragma once
#include <unordered_set>
#include <functional>
#include <typeinfo>

#include <fustd/generic/function_traits.hpp>

FUSTD_BEGIN_NAMESPACE

namespace details {
struct FunctionHasher {
	template<class... ArgsType>
	std::size_t operator()(const std::function<void(ArgsType...)>& val) const {
		return val.target_type().hash_code();
	}
};

struct FunctionEqualTo {
	template<class... ArgsType>
	bool operator()(const std::function<void(ArgsType...)>& x, const std::function<void(ArgsType...)>& y) const {
		return x.target_type() == y.target_type();
	}
};

template<class... ArgsType>
using function_set = std::unordered_set<std::function<void(ArgsType...)>, details::FunctionHasher, details::FunctionEqualTo>;
}


template<class... ArgsType>
class Action
{
public:
	Action() {}
	template<class FuncType>
		requires is_all_same_args_func_v<FuncType, void(ArgsType...)>
	Action(FuncType&& func) {
		AddFunc(std::forward<FuncType>(func));
	}
	template<class FuncType>
		requires is_all_same_args_func_v<FuncType, void(ArgsType...)>
	Action& operator=(FuncType&& func) {
		func_set_.clear();
		AddFunc(std::forward<FuncType>(func));
		return *this;
	}
	~Action() {}

	template<class FuncType>
		requires is_all_same_args_func_v<FuncType, void(ArgsType...)>
	Action& operator+=(FuncType&& func) {
		AddFunc(std::forward<FuncType>(func));
		return *this;
	}

	template<class FuncType>
		requires is_all_same_args_func_v<FuncType, void(ArgsType...)>
	Action& operator-=(FuncType&& func) {
		RemoveFunc(std::forward<FuncType>(func));
		return *this;
	}

	void Invoke(ArgsType&&... args) {
		for (auto& func_ : func_set_) {
			func_(std::forward<ArgsType>(args)...);
		}
	}

private:
	template<class FuncType>
		requires is_all_same_args_func_v<FuncType, void(ArgsType...)>
	void AddFunc(FuncType&& func) {
		func_set_.insert(std::forward<FuncType>(func));
	}
	template<class FuncType>
		requires is_all_same_args_func_v<FuncType, void(ArgsType...)>
	void RemoveFunc(FuncType&& func) {
		func_set_.erase(std::forward<FuncType>(func));
	}

	details::function_set<ArgsType...> func_set_;
};

FUSTD_END_NAMESPACE
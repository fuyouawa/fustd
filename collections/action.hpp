#pragma once
#include <unordered_set>
#include <functional>
#include <typeinfo>

#include <fustd/generic/function_traits.hpp>

namespace fustd {
namespace details {
struct FunctionHasher {
	template<class... ArgsT>
	std::size_t operator()(const std::function<void(ArgsT...)>& val) const {
		return val.target_type().hash_code();
	}
};

struct FunctionEqualTo {
	template<class... ArgsT>
	bool operator()(const std::function<void(ArgsT...)>& x, const std::function<void(ArgsT...)>& y) const {
		return x.target_type() == y.target_type();
	}
};

template<class... ArgsT>
using function_set = std::unordered_set<std::function<void(ArgsT...)>, details::FunctionHasher, details::FunctionEqualTo>;
}


template<class... ArgsT>
class Action
{
public:
	template<class FuncT>
	using enable_if_same_args = std::enable_if_t<is_all_same_args_func_v<FuncT, void(ArgsT...)>, int>;

	Action() {}
	template<class FuncT>
	Action(FuncT&& func, enable_if_same_args<FuncT> = 0) {
		AddFunc(std::forward<FuncT>(func));
	}
	template<class FuncT, enable_if_same_args<FuncT> = 0>
	Action& operator=(FuncT&& func) {
		func_set_.clear();
		AddFunc(std::forward<FuncT>(func));
		return *this;
	}
	~Action() {}

	template<class FuncT, enable_if_same_args<FuncT> = 0>
	Action& operator+=(FuncT&& func) {
		AddFunc(std::forward<FuncT>(func));
		return *this;
	}

	template<class FuncT, enable_if_same_args<FuncT> = 0>
	Action& operator-=(FuncT&& func) {
		RemoveFunc(std::forward<FuncT>(func));
		return *this;
	}

	void Invoke(ArgsT&&... args) {
		for (auto& func_ : func_set_) {
			func_(std::forward<ArgsT>(args)...);
		}
	}

private:
	template<class FuncT, enable_if_same_args<FuncT> = 0>
	void AddFunc(FuncT&& func) {
		func_set_.insert(std::forward<FuncT>(func));
	}
	template<class FuncT, enable_if_same_args<FuncT> = 0>
	void RemoveFunc(FuncT&& func) {
		func_set_.erase(std::forward<FuncT>(func));
	}

	details::function_set<ArgsT...> func_set_;
};
}
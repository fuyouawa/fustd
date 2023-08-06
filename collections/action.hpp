#pragma once
#include <unordered_set>
#include <functional>
#include <typeinfo>

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
	using FuncT = std::function<void(ArgsT...)>;

	Action() {}
	Action(const FuncT& func) {
		AddFunc(func);
	}
	Action& operator=(const FuncT& func) {
		func_set_.clear();
		AddFunc(func);
		return *this;
	}
	~Action() {}

	Action& operator+=(const FuncT& func) {
		AddFunc(func);
		return *this;
	}

	Action& operator-=(const FuncT& func) {
		RemoveFunc(func);
		return *this;
	}

	void Invoke(ArgsT&&... args) {
		for (auto& func_ : func_set_) {
			func_(std::forward<ArgsT>(args)...);
		}
	}

private:
	void AddFunc(const FuncT& func) {
		func_set_.insert(func);
	}
	void RemoveFunc(const FuncT& func) {
		func_set_.erase(func);
	}

	details::function_set<ArgsT...> func_set_;
};
}
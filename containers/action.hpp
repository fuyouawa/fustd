#pragma once
#include <functional>
#include <concepts>
#include <unordered_set>

namespace fustd {

template<typename... Args>
class Action {
public:
	Action() = default;
	~Action() = default;

	template<typename... NewArgs>
	requires (sizeof...(NewArgs) == sizeof...(Args))
	void Invoke(NewArgs&&... args) {
		for (auto& action : actions_) {
			action(std::forward<NewArgs>(args)...);
		}
	}

	Action& operator+=(std::invocable<Args...> auto action) {
		actions_.insert(action);
	}

private:
	struct FuncHasher {
		template<typename... Args>
		std::size_t operator()(const std::function<Args...>& func) const {
			return std::hash{}(func.target_type().name());
		}
	};

	std::unordered_set<std::function<void(Args...)>, FuncHasher> actions_;
};
}
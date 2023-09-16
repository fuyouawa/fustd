#pragma once
#include <typeinfo>
#include <unordered_set>

#include <fustd/collections/details/type_wrapper.hpp>
#include <fustd/generic/option.hpp>

// 不可用于驱动使用
#ifndef WINNT
namespace fustd {
class TypeHashSet
{
public:
	using Wrapper = details::TypeWrapper<void>;

	TypeHashSet(): total_() {}
	template<class... Types>
	TypeHashSet(Types*... vals) {
		(Insert(vals), ...);
	}
	~TypeHashSet() = default;

	template<class T>
	void Insert(T* val) {
		if (!Contains<T>())
			total_.insert(Wrapper(val, true));
	}

	template<class T>
	bool Erase() {
		auto found = total_.find(Wrapper::FromT<T>());
		if (found != total_.end()) {
			total_.erase(found);
			return true;
		}
		return false;
	}

	template<class T>
	bool Contains() const noexcept {
		return total_.find(Wrapper::FromT<T>()) != total_.end();
	}

	bool Empty() const noexcept {
		return total_.empty();
	}

	size_t Size() const noexcept {
		return total_.size();
	}

	void Clear() {
		total_.clear();
	}

	template<class T>
	Option<T*> Find() const noexcept {
		auto found = total_.find(Wrapper::FromT<T>());
		if (found)
			return Some(found->Value<T>());
		return None;
	}

private:
	std::unordered_set<Wrapper, Wrapper::TypeHasher, Wrapper::TypeEqualTo> total_;
};
}
#endif // !WINNT
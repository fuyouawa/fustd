#pragma once
#include <typeinfo>
#include <unordered_set>

#include <fustd/generic/type.hpp>
#include <fustd/generic/option.hpp>

namespace fustd {
class TypeSet
{
public:
	TypeSet(): total_() {}
	template<class... Types>
	TypeSet(Types*... vals) {
		(Insert(vals), ...);
	}
	~TypeSet() {}

	template<class T>
	void Insert(T* val) {
		if (!Contains<T>()) {
			total_.insert(val);
		}
	}

	template<class T>
	bool Erase() {
		auto found = total_.find(Type::FromT<T>());
		if (found != total_.end()) {
			total_.erase(found);
			return true;
		}
		return false;
	}

	template<class T>
	bool Contains() const noexcept {
		return total_.find(Type::FromT<T>()) != total_.end();
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
		auto found = total_.find(Type::FromT<T>());
		if (found)
			return Some(found->Value<T>());
		return None;
	}

private:
	std::unordered_set<Type, Type::TypeHasher, Type::TypeEqualTo> total_;
};
}
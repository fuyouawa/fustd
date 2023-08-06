#pragma once
#include <unordered_set>
#include <fustd/generic/type.hpp>
#include <fustd/generic/option.hpp>

namespace fustd {
class ObjectSet
{
public:
	ObjectSet() : total_() {}
	ObjectSet(std::initializer_list<Type> vals) {
		for (auto& v : vals) {
			Insert(v);
		}
	}
	~ObjectSet() {}

	void Insert(const Type& val) {
		total_.insert(val);
	}

	bool Erase(const Type& val) {
		auto found = total_.find(val);
		if (found != total_.end()) {
			total_.erase(found);
			return true;
		}
		return false;
	}

	bool Contains(const Type& val) const noexcept {
		return total_.find(val) != total_.end();
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
	Option<T*> Find(T* val) const noexcept {
		auto found = total_.find(val);
		if (found != total_.end())
			return Some(found->Value<T>());
		return None;
	}

private:
	std::unordered_set<Type, Type::ValueHasher, Type::ValueEqualTo> total_;
};
}
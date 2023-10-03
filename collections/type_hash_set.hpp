#pragma once
#include <typeinfo>
#include <unordered_set>
#include <optional>

#include <fustd/collections/details/type_wrapper.hpp>

FUSTD_BEGIN_NAMESPACE

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
	std::optional<T*> Find() const noexcept {
		auto found = total_.find(Wrapper::FromT<T>());
		if (found)
			return found->Value<T>();
		return std::nullopt;
	}

private:
	std::unordered_set<Wrapper, Wrapper::TypeHasher, Wrapper::TypeEqualTo> total_;
};

FUSTD_END_NAMESPACE
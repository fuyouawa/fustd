#pragma once
#include <unordered_set>
#include <optional>

#include <fustd/collections/details/type_wrapper.hpp>

FUSTD_BEGIN_NAMESPACE

class ObjectHashSet
{
public:
	using Wrapper = details::TypeWrapper<void>;

	ObjectHashSet() : total_() {}
	template<class... Types>
	ObjectHashSet(Types*... vals) {
		(Insert(vals), ...);
	}
	~ObjectHashSet() = default;

	template<class T>
	void Insert(T* val) {
		if (!Contains(val))
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
	bool Contains(T* val) const noexcept {
		return total_.find(Wrapper(val, false)) != total_.end();
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
	std::optional<T*> Find(T* val) const noexcept {
		auto found = total_.find(Wrapper(val, false));
		if (found != total_.end())
			return found->Value<T>();
		return std::nullopt;
	}

private:
	std::unordered_set<Wrapper, Wrapper::ValueHasher, Wrapper::ValueEqualTo> total_;
};

FUSTD_END_NAMESPACE

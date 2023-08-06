#pragma once
#include <unordered_set>

#include <fustd/collections/details/type_wrapper.hpp>

namespace fustd {
template<class T>
class ObjectList {
public:
	using Wrapper = details::TypeWrapper<T>;

	ObjectList() : total_() {}
	template<class... Types>
	ObjectList(Types*... vals) {
		(Insert(vals), ...);
	}
	~ObjectList() = default;

	template<class U>
	void Insert(U* val) {
		if (!Contains(val)) {
			total_.insert(Wrapper(val, true));
		}
	}

	template<class U>
	bool Erase(U* val) {
		auto found = total_.find(Wrapper(val, false));
		if (found != total_.end()) {
			total_.erase(found);
			return true;
		}
		return false;
	}

	template<class U>
	bool Contains(U* val) const noexcept {
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

	template<class U>
	Option<U*> Find(U* val) const noexcept {
		auto found = total_.find(Wrapper(val, false));
		if (found)
			return Some(found->Value<U>());
		return None;
	}
private:
	std::unordered_set<Wrapper, typename Wrapper::TypeHasher, typename Wrapper::BothTypeAndValueEqualTo> total_;
};
}
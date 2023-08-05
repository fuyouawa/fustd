#pragma once
#include <typeinfo>
#include <unordered_map>
#include <functional>
#include <fystd/option.hpp>

namespace fystd {
namespace details {
class TypeInfoWrapper
{
public:
	TypeInfoWrapper(const type_info& info) noexcept : info_ptr_(std::addressof(info)) {}
	~TypeInfoWrapper() noexcept {}

	const type_info* info_ptr_;
};
struct TypeInfoWrapperHasher {
	std::size_t operator()(const TypeInfoWrapper& x) const noexcept {
		return x.info_ptr_->hash_code();
	}
};
struct TypeInfoWrapperEqualTo {
	bool operator()(const TypeInfoWrapper& x, const TypeInfoWrapper& y) const noexcept {
		return *x.info_ptr_ == *y.info_ptr_;
	}
};
struct DefaultTypeInfoGetter {
	template<class T>
	const std::type_info& operator()(T) const noexcept {
		return typeid(T);
	}
};
}

template<class T, class TypeInfoGetter=details::DefaultTypeInfoGetter>
class HashList
{
public:
	using TDeleter = std::function<void(void*)>;
	using Container = std::unordered_map<details::TypeInfoWrapper, std::pair<T*, TDeleter>, details::TypeInfoWrapperHasher, details::TypeInfoWrapperEqualTo>;
	using ContainerIterator = typename Container::iterator;
	template<class U>
	using EnableIfConvertibleT = std::enable_if_t<std::is_convertible_v<U*, T*>, int>;

	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		reference operator*() const { return (*iter_).second.first; }
		pointer operator->() const { return &(operator*()); }
		Iterator& operator++() {
			iter_++;
			return *this;
		}
		Iterator operator++(int) {
			Iterator tmp = *this;
			++*this;
			return tmp;
		}
		friend bool operator==(const Iterator& a, const Iterator& b) const noexcept { return a.iter_ == b.iter_; }
		friend bool operator!=(const Iterator& a, const Iterator& b) const noexcept { return a.iter_ != b.iter_; }

	private:
		friend class HashList;
		Iterator() {}
		Iterator(const ContainerIterator& iter) {
			iter_ = iter;
		}
		ContainerIterator iter_;
	};

	HashList(): typeinfo_getter_(), container_() {}
	template<class... Types>
	HashList(Types*... args) {
		(Insert(args), ...);
	}
	~HashList() {
		for (auto& i : container_) {
			DeleteValue(i.second);
		}
	}

	template<class U, EnableIfConvertibleT<U> = 0>
	void Insert(U* val) {
		auto found = container_.find(GetType<U>());
		if (found != container_.end()) {
			DeleteValue(found->second);
		}
		container_[GetType<U>()] = { dynamic_cast<T*>(val), [](void* ptr) { delete static_cast<U*>(ptr); } };
	}

	template<class U, EnableIfConvertibleT<U> = 0>
	bool Erase() {
		auto found = container_.find(GetType<U>());
		if (found != container_.end()) {
			container_.erase(found);
			return true;
		}
		return false;
	}

	template<class U, EnableIfConvertibleT<U> = 0>
	Iterator Find() const noexcept {
		return Iterator(container_.find(GetType<U>()));
	}

	template<class U, EnableIfConvertibleT<U> = 0>
	bool Contain() const noexcept {
		return container_.find(GetType<U>()) != container_.end();
	}

	bool Empty() const noexcept {
		return container_.empty();
	}

	size_t Size() const noexcept {
		return container_.size();
	}

	void Clear() {
		container_.clear();
	}

	template<class U, EnableIfConvertibleT<U> = 0>
	fystd::Option<U> GetValue() const noexcept {
		auto found = container_.find(GetType<U>());
		if (found)
			return fystd::Some(found.second.first);
		return fystd::None;
	}
	
	Iterator begin() { return Iterator(container_.begin()); }
	Iterator end() { return Iterator(container_.end()); }

	Iterator begin() const { return Iterator(container_.begin()); }
	Iterator end() const { return Iterator(container_.end()); }

private:
	template<class U>
	const std::type_info& GetType() const noexcept {
		return typeinfo_getter_((U*)0);
	}

	void DeleteValue(std::pair<T*, TDeleter>& x) {
		if (x.first) {
			x.second(x.first);
		}
		x.first = nullptr;
	}

	TypeInfoGetter typeinfo_getter_;
	Container container_;
};
}
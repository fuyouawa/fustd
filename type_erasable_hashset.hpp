#pragma once
#include <typeinfo>
#include <unordered_map>
#include <fystd/option.hpp>

namespace easyfwk {
namespace details {
class TypeInfoWrapper
{
public:
	TypeInfoWrapper(const type_info& info) : info_ptr_(std::addressof(info)) {}
	~TypeInfoWrapper() {}

	const type_info* info_ptr_;
};

struct TypeInfoWrapperHasher {
	std::size_t operator()(const TypeInfoWrapper& x) const {
		return x.info_ptr_->hash_code();
	}
};

struct TypeInfoWrapperEqualTo {
	bool operator()(const TypeInfoWrapper& x, const TypeInfoWrapper& y) const {
		return *x.info_ptr_ == *y.info_ptr_;
	}
};

struct DefaultTypeInfoGetter {
	template<class T>
	const std::type_info& operator()(T) const {
		return typeid(T);
	}
};
}

template<class T, class TypeInfoGetter=details::DefaultTypeInfoGetter>
class TypeErasableHashSet
{
public:
	using TDeleterPtr = void(*)(void* ptr);
	using Container = std::unordered_map<details::TypeInfoWrapper, std::pair<T*, TDeleterPtr>, details::TypeInfoWrapperHasher, details::TypeInfoWrapperEqualTo>;
	using ContainerIterator = typename Container::iterator;

	template<class U>
	using EnableIfConvertibleT = std::enable_if_t<std::is_convertible_v<U*, T*>, int>;

	TypeErasableHashSet(): typeinfo_getter_(), container_() {}
	TypeErasableHashSet(std::initializer_list<T*> init) {

	}
	~TypeErasableHashSet() {
		for (auto& i : container_) {
			if (i.second.first) {
				i.second.second(i.second.first);
			}
		}
	}

	template<class U, EnableIfConvertibleT<U> = 0>
	void Insert(U* val) {
		container_.insert(
			{ 
			GetType<U>(),
			{
				dynamic_cast<T*>(val),
				[](void* ptr) { static_cast<U*>(ptr)->~U(); }
			}
			}
		);
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

	void Clear() {
		container_.clear();
	}

	template<class U, EnableIfConvertibleT<U> = 0>
	fystd::Option<U> GetValue() {
		auto found = container_.find(GetType<U>());
		if (found)
			return fystd::Some(found.second.first);
		return fystd::None;
	}

	class iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		reference operator*() const { return (*iter_).second.first; }

		pointer operator->() const { return &(operator*()); }

		iterator& operator++() {
			iter_++;
			return *this;
		}

		iterator operator++(int) {
			iterator tmp = *this;
			++*this;
			return tmp;
		}

		friend bool operator==(const iterator& a, const iterator& b) {
			return a.iter_ == b.iter_;
		}

		friend bool operator!=(const iterator& a, const iterator& b) {
			return a.iter_ != b.iter_;
		}

	private:
		friend class TypeErasableHashSet;
		iterator() {}
		iterator(const ContainerIterator& iter) {
			iter_ = iter;
		}
		ContainerIterator iter_;
	};
	
	iterator begin() { return iterator(container_.begin()); }
	iterator end() { return iterator(container_.end()); }

	iterator begin() const { return iterator(container_.begin()); }
	iterator end() const { return iterator(container_.end()); }

private:
	template<class U>
	std::type_info& GetType() {
		return typeinfo_getter_((U*)0);
	}

	TypeInfoGetter typeinfo_getter_;
	Container container_;
};
}
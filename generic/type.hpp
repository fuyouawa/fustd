#pragma once
#include <type_traits>
#include <typeinfo>

namespace fustd {
class Type
{
public:
	struct ValueHasher {
		_NODISCARD size_t operator()(const Type& t) const noexcept {
			return t.ValueHashCode();
		}
	};
	struct ValueEqualTo {
		_NODISCARD size_t operator()(const Type& x, const Type& y) const noexcept {
			return x.val_ == y.val_;
		}
	};
	struct TypeHasher {
		_NODISCARD size_t operator()(const Type& t) const noexcept {
			return t.TypeHashCode();
		}
	};
	struct TypeEqualTo {
		_NODISCARD size_t operator()(const Type& x, const Type& y) const noexcept {
			return *x.typeinfo_ == *y.typeinfo_;
		}
	};

	template<class T>
	static constexpr Type FromT() {
		return Type((T*)0);
	}

	template <class T>
	constexpr Type(T* val) noexcept :
		val_(val),
		deleter_([](void* ptr) { delete static_cast<T*>(ptr); }),
		typeinfo_(&typeid(T)),
		ref_count_ptr_(new size_t(0)){}
	~Type() noexcept {
		if (RefCount() == 0)
			Tidy();
		else
			DesRefCount();
	}

	template<class U>
	void EqualType() {
		return typeid(U) == *typeinfo_;
	}

	template<class U>
	void EqualValue(U* val) {
		return static_cast<U*>(val_) == val;
	}

	void Swap(Type& rvalue) noexcept {
		auto val_tmp_ = val_;
		auto deleter_tmp_ = deleter_;
		auto typeinfo_tmp_ = typeinfo_;
		auto ref_count_tmp_ = ref_count_ptr_;
		val_ = rvalue.val_;
		deleter_ = rvalue.deleter_;
		typeinfo_ = rvalue.typeinfo_;
		ref_count_ptr_ = rvalue.ref_count_ptr_;
		rvalue.val_ = val_tmp_;
		rvalue.deleter_ = deleter_tmp_;
		rvalue.typeinfo_ = typeinfo_tmp_;
		rvalue.ref_count_ptr_ = ref_count_tmp_;
	}

	template<class T>
	T* Value() const noexcept {
		return static_cast<T*>(val_);
	}

	const char* Name() const noexcept {
		return typeinfo_->name();
	}

	std::size_t TypeHashCode() const noexcept {
		return typeinfo_->hash_code();
	}

	std::size_t ValueHashCode() const noexcept {
		return std::hash<void*>()(val_);
	}

	Type(const Type& lvalue) noexcept {
		ResetCopy(lvalue);
	}

	Type(Type&& rvalue) noexcept {
		ResetMove(std::move(rvalue));
	}

	Type& operator=(const Type& lvalue) noexcept {
		if (this != std::addressof(lvalue))
			Type(lvalue).Swap(*this);
		return *this;
	}

	Type& operator=(Type&& rvalue) noexcept {
		if (this != std::addressof(rvalue))
			Type(std::move(rvalue)).Swap(*this);
		return *this;
	}

private:
	using TDeleterPtr = void(*)(void*);

	void ResetCopy(const Type& rvalue) noexcept {
		val_ = rvalue.val_;
		deleter_ = rvalue.deleter_;
		typeinfo_ = rvalue.typeinfo_;
		ref_count_ptr_ = rvalue.ref_count_ptr_;
		InsRefCount();
	}

	void ResetMove(Type&& rvalue) noexcept {
		ResetCopy(rvalue);
		rvalue.val_ = nullptr;
		rvalue.deleter_ = nullptr;
		rvalue.typeinfo_ = nullptr;
	}

	void Tidy() noexcept {
		if (val_ && deleter_) {
			deleter_(val_);
		}
		if (ref_count_ptr_) {
			delete ref_count_ptr_;
		}
		val_ = nullptr;
		typeinfo_ = nullptr;
		deleter_ = nullptr;
	}

	void InsRefCount() noexcept {
		if (ref_count_ptr_)
			++*ref_count_ptr_;
	}

	void DesRefCount() noexcept {
		if (ref_count_ptr_)
			--*ref_count_ptr_;
	}

	size_t RefCount() const noexcept {
		return *ref_count_ptr_;
	}

	size_t* ref_count_ptr_;
	void* val_;
	TDeleterPtr deleter_;
	const std::type_info* typeinfo_;
};
}
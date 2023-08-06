#pragma once
#include <typeinfo>
#include <fustd/generic/type_traits.hpp>

namespace fustd {
namespace details {
template<class T>
class TypeWrapper
{
public:
	struct ValueHasher {
		_NODISCARD size_t operator()(const TypeWrapper& t) const noexcept {
			return t.ValueHashCode();
		}
	};
	struct ValueEqualTo {
		_NODISCARD bool operator()(const TypeWrapper& x, const TypeWrapper& y) const noexcept {
			if constexpr (has_equality_operator_v<T>)
				return *x.val_ptr_ == *y.val_ptr_;
			else
				return x.val_ptr_ == y.val_ptr_;
		}
	};
	struct TypeHasher {
		_NODISCARD size_t operator()(const TypeWrapper& t) const noexcept {
			return t.TypeHashCode();
		}
	};
	struct TypeEqualTo {
		_NODISCARD bool operator()(const TypeWrapper& x, const TypeWrapper& y) const noexcept {
			return *x.typeinfo_ == *y.typeinfo_;
		}
	};
	struct BothTypeAndValueEqualTo {
		_NODISCARD bool operator()(const TypeWrapper& x, const TypeWrapper& y) const noexcept {
			return ValueEqualTo()(x, y) && TypeEqualTo()(x, y);
		}
	};

	template<class U>
	using enable_if_conv_to_t = std::enable_if_t<std::is_convertible_v<U*, T*>, int>;

	template<class U, enable_if_conv_to_t<U> = 0>
	static constexpr TypeWrapper FromT() {
		return TypeWrapper((U*)0, false);
	}

	template <class U, enable_if_conv_to_t<U> = 0>
	constexpr TypeWrapper(U* val, bool auto_del=false) noexcept :
		val_ptr_(val),
		deleter_([](void* ptr) { delete static_cast<U*>(ptr); }),
		typeinfo_(&typeid(U)),
		auto_del_(auto_del) {}

	~TypeWrapper() noexcept {
		if (auto_del_)
			Tidy();
	}

	template<class U, enable_if_conv_to_t<U> = 0>
	void EqualType() {
		return typeid(U) == *typeinfo_;
	}

	template<class U, enable_if_conv_to_t<U> = 0>
	void EqualValue(U* val) {
		if constexpr (has_equality_operator_v<U>)
			return *dynamic_cast<U*>(val_ptr_) == *val;
		else
			return dynamic_cast<U*>(val_ptr_) == val;
	}

	void Swap(TypeWrapper& rvalue) noexcept {
		auto val_tmp_ = val_ptr_;
		auto deleter_tmp_ = deleter_;
		auto typeinfo_tmp_ = typeinfo_;
		val_ptr_ = rvalue.val_ptr_;
		deleter_ = rvalue.deleter_;
		typeinfo_ = rvalue.typeinfo_;
		rvalue.val_ptr_ = val_tmp_;
		rvalue.deleter_ = deleter_tmp_;
		rvalue.typeinfo_ = typeinfo_tmp_;
	}

	void Tidy() noexcept {
		if (val_ptr_ && deleter_) {
			deleter_(val_ptr_);
		}
		val_ptr_ = nullptr;
		typeinfo_ = nullptr;
		deleter_ = nullptr;
	}

	template<class U, enable_if_conv_to_t<U> = 0>
	U* Value() const noexcept {
		return static_cast<U*>(val_ptr_);
	}

	const char* Name() const noexcept {
		return typeinfo_->name();
	}

	std::size_t TypeHashCode() const noexcept {
		return typeinfo_->hash_code();
	}

	std::size_t ValueHashCode() const noexcept {
		return std::hash<void*>()(val_ptr_);
	}

	TypeWrapper(const TypeWrapper& lvalue) noexcept {
		ResetCopy(lvalue);
	}

	TypeWrapper(TypeWrapper&& rvalue) noexcept {
		ResetMove(std::move(rvalue));
	}

	TypeWrapper& operator=(const TypeWrapper& lvalue) noexcept {
		if (this != std::addressof(lvalue))
			TypeWrapper(lvalue).Swap(*this);
		return *this;
	}

	TypeWrapper& operator=(TypeWrapper&& rvalue) noexcept {
		if (this != std::addressof(rvalue))
			TypeWrapper(std::move(rvalue)).Swap(*this);
		return *this;
	}

private:
	using TDeleterPtr = void(*)(void*);

	void ResetCopy(const TypeWrapper& rvalue) noexcept {
		val_ptr_ = rvalue.val_ptr_;
		deleter_ = rvalue.deleter_;
		typeinfo_ = rvalue.typeinfo_;
	}

	void ResetMove(TypeWrapper&& rvalue) noexcept {
		ResetCopy(rvalue);
		rvalue.val_ptr_ = nullptr;
		rvalue.deleter_ = nullptr;
		rvalue.typeinfo_ = nullptr;
	}

	bool auto_del_;
	T* val_ptr_;
	TDeleterPtr deleter_;
	const std::type_info* typeinfo_;
};
}
}
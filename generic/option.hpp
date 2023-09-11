#pragma once
#include <type_traits>

namespace fustd {
template <class T>
class Some {
public:
	Some(const Some&) = delete;
	template<class T1>
	Some(Some<T1>&& rvalue) : val_(std::move(rvalue.val_)) {
		static_assert(std::is_convertible_v<T1, T>, "T1 must be T, or it can be implicitly converted to T!");
	}
	Some(T&& value) : val_(std::forward<T>(value)) { }
	~Some() {}
	T val_;
};
struct None {};

/**
 * @brief 抽象可空类型的理念, Option表示可空类型, Some表示有值, None表示空
 * @tparam T Some的数据类型
*/
template<class T>
class Option
{
public:
	enum Tag {
		kSomeTag,
		kNoneTag
	};
	/**
	 * @brief 接收Some, 表示有值
	 * @param rvalue 右值
	*/
	template<class T1>
	Option(Some<T1>&& rvalue) : some_(std::move(rvalue)) { tag_ = kSomeTag; }
	/**
	 * @brief 接收None, 表示空
	*/
	Option(None) { tag_ = kNoneTag; }
	~Option() {
		if (IsSome()) {
			some_.~Some();
		}
	}

	/**
	 * @brief 是否有值
	*/
	bool IsSome() const noexcept {
		return tag_ == kSomeTag;
	}

	/**
	 * @brief 是否为空
	*/
	bool IsNone() const noexcept {
		return !IsSome();
	}

	/**
	 * @brief 获取值
	*/
	T SomeVal() const noexcept {
		return some_.val_;
	}

	/**
	 * @brief 如果有值, 返回数据; 否则返回设定的默认值
	 * @param val 默认值
	*/
	T Default(const T& val) const noexcept {
		if (IsSome())
			return SomeVal();
		else
			return val;
	}

private:
	char tag_;
	union {
		Some<T> some_;
	};
};
}
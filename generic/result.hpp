#pragma once
#include <type_traits>
#include <stdexcept>

namespace fustd {
template <class T>
class Ok {
public:
	Ok(const Ok&) = delete;
	template<class T1>
	Ok(Ok<T1>&& rvalue) : val_(std::move(rvalue.val_)) {
		static_assert(std::is_convertible_v<T1, T>, "T1 must be T, or it can be implicitly converted to T!");
	}
	Ok(T&& value) : val_(std::forward<T>(value)) { }
	~Ok() {}
	T val_;
};

template <class T>
class Err {
public:
	Err(const Err&) = delete;
	template<class T1>
	Err(Err<T1>&& rvalue) : val_(std::move(rvalue.val_)) {
		static_assert(std::is_convertible_v<T1, T>, "T1 must be T, or it can be implicitly converted to T!");
	}
	Err(T&& value) : val_(std::forward<T>(value)) { }
	~Err() {}
	T val_;
};

/**
 * @brief 抽象可恢复错误概念, Result表示可恢复错误类型, Ok表示成功, Err表示错误
 * @tparam T Ok的数据类型
 * @tparam E Err的错误信息类型
*/
template<class T, class E>
class Result
{
public:
	enum Tag {
		kOkTag,
		kErrTag
	};

	/**
	 * @brief 接收Ok, 表示函数执行成功
	 * @param rvalue 右值
	*/
	template<class T1>
	Result(Ok<T1>&& rvalue) : ok_(std::move(rvalue)) { tag_ = kOkTag; }
	/**
	 * @brief 接收Ok, 表示有错误
	 * @param rvalue 右值
	*/
	template<class E1>
	Result(Err<E1>&& rvalue) : err_(std::move(rvalue)) { tag_ = kErrTag; }
	~Result() {
		if (IsOk())
			ok_.~Ok();
		else
			err_.~Err();
	}

	/**
	 * @brief 是否执行成功
	*/
	bool IsOk() const noexcept {
		return tag_ == kOkTag;
	}

	/**
	 * @brief 是否出现错误
	*/
	bool IsErr() const noexcept {
		return !IsOk();
	}

	/**
	 * @brief 获取函数想要返回的数据
	*/
	T OkVal() const noexcept {
		return ok_.val_;
	}

	/**
	 * @brief 获取错误信息
	*/
	E ErrVal() const noexcept {
		return err_.val_;
	}

	/**
	 * @brief 如果Ok, 返回Ok中的数据; 否则调用参数op并传入Err中的数据
	 * @tparam FuncT 可调用对象
	 * @param op 可调用对象, 接收Err的数据
	*/
	template<class FuncT, std::enable_if_t<std::is_invocable_v<FuncT>, int> = 0>
	T UnwrapOrElse(const FuncT& op) const {
		if (IsOk())
			return OkVal();
		else
			op(ErrVal());
	}

	/**
	 * @brief 如果Ok, 返回Ok中的数据; 否则抛出异常
	 * @param msg 异常信息
	*/
	T Expect(const char* msg) const {
		if (IsOk())
			return OkVal();
		else
			std::_Xruntime_error(msg);
	}

	/**
	 * @brief 如果Ok, 返回Ok中的数据; 否则抛出异常
	*/
	T Unwrap() const {
		if (IsOk())
			return OkVal();
		else
			std::_Xruntime_error("Error occurred!");
	}

	/**
	 * @brief 如果Ok, 返回Ok中的数据; 否则返回设定的默认值
	 * @param val 默认值
	*/
	T Default(const T& val) const noexcept {
		if (IsOk())
			return OkVal();
		else
			return val;
	}

private:
	char tag_;
	union {
		Ok<T> ok_;
		Err<E> err_;
	};
};
}

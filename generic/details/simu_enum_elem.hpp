#pragma once
#include <fustd/generic/type_traits.hpp>

namespace fustd {
namespace details {
template <class T, class Tag>
class SimuEnumElem
{
public:
	SimuEnumElem(const SimuEnumElem&) = delete;
	template<class E>
	SimuEnumElem(SimuEnumElem<E, Tag>&& rvalue) : val_(std::move(rvalue.val_)) {
		static_assert(std::is_convertible_v<E, T>, "U must be T, or it can be implicitly converted to T!"); \
	}
	SimuEnumElem(T&& value) : val_(std::forward<T>(value)) { }
	~SimuEnumElem() {}
	T val_;
};
}
}
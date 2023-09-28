#pragma once
#include <stdint.h>

namespace fustd {
	class Varient
	{
	public:
		Varient(void* ptr) : ptr_(ptr) {}
		~Varient() = default;

		uint64_t& u64() const noexcept {
			return Value<uint64_t>();
		}

		uint32_t& u32() const noexcept {
			return Value<uint32_t>();
		}

		uint16_t& u16() const noexcept {
			return Value<uint16_t>();
		}

		uint8_t& u8() const noexcept {
			return Value<uint8_t>();
		}

		int64_t& i64() const noexcept {
			return Value<int64_t>();
		}

		int32_t& i32() const noexcept {
			return Value<int32_t>();
		}

		int16_t& i16() const noexcept {
			return Value<int16_t>();
		}

		int8_t& i8() const noexcept {
			return Value<int8_t>();
		}

		template<class T>
		T& obj() const noexcept {
			return Value<T>();
		}

		void Reset(void* ptr) noexcept {
			ptr_ = ptr;
		}


	private:
		template<class T>
		T& Value() const noexcept {
			return *reinterpret_cast<T*>(ptr_);
		}

		void* ptr_;
	};
}
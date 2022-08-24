#pragma once

#include <cassert>
#include <utility>

// In modern C++ there views with different semantic:
// T&
//   - non-owning
//   - exactly one valid T
// T*
//   - non-owning
//   - 0 or 1 valid T
// std::unique/shared_ptr<T>
//   - OWNING 
//   - 0 or 1 valid T
// std::span<T>
//   - non owning
//   - contiguous in memory some count of T's
// pair iterator + sentinel(views)
//   - non owning
//   - some count of T's
// containers(ranges)
//   - OWNING
//   - things with some count of T
// 
// But many people still make mistakes such as:
//   - owner raw pointer
//   - pointer in signature when you dont expect nullptr
//   - pointer to more then one value like void foo(T*, size_t) (it must be void foo(std::span<T>) )
//
// So this header implements replacement for last semantic of T* in C++
// maybe<T> cant be deleted(-1 error)
// no pointer ariphmetic or operator[], so <= 1 value always semantically
// it is obvious in signature of function, that maybe<T> may be null
namespace art {

  // non-owning nullable view to 1(or 0) valid T
  template <typename T>
  struct maybe {
   private:
    T* value_ptr = nullptr;  // invalid for references, and it is correct maybe<T&> is semanticly bad
   public:
    maybe() = default;
    constexpr maybe(T* p) noexcept : value_ptr(p) {
    }
    constexpr maybe(std::nullptr_t) noexcept : maybe() {
    }
    constexpr bool operator==(std::nullptr_t) const noexcept {
      return value_ptr == nullptr;
    }
    constexpr maybe& operator=(T* p) noexcept {
      value_ptr = p;
      return *this;
    }
    constexpr maybe& operator=(std::nullptr_t) noexcept {
      value_ptr = nullptr;
      return *this;
    }
    constexpr T* operator->() noexcept {
      assert(value_ptr != nullptr);
      return value_ptr;
    }
    constexpr const T* operator->() const noexcept {
      assert(value_ptr != nullptr);
      return value_ptr;
    }
    constexpr T& operator*() noexcept {
      assert(value_ptr != nullptr);
      return *value_ptr;
    }
    constexpr const T& operator*() const noexcept {
      assert(value_ptr != nullptr);
      return *value_ptr;
    }
  };

}  // namespace art
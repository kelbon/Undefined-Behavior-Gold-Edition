#pragma once

#include <type_traits>
#include <voidify.hpp>

// implements array without C array, which means array in core C++ is useless
namespace art {

  template <typename T, size_t>
  struct value {
    T v;
  };

  template<typename,typename>
  struct array_impl;

  template <typename T, size_t... Is>
  struct array_impl<T, std::index_sequence<Is...>> : value<T, Is>... {
    using value<T, Is>::value...;
    constexpr array_impl() = default;
    // clang-format off
    template <typename... Ts>
    requires(sizeof...(Ts) == sizeof...(Is))
    constexpr array_impl(Ts&&... args) noexcept((std::is_nothrow_constructible_v<T, Ts&&> && ...))
        : value<T, Is>{std::forward<Ts>(args)}... {
    }
    template <typename... Ts>
    requires(sizeof...(Ts) < sizeof...(Is) && std::is_default_constructible_v<T>)
    constexpr array_impl(std::initializer_list<T> l)
        noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_default_constructible_v<T>)
        : value<T, Is>{Is < l.size() ? *std::next(l.begin(), Is) : T{}}... {
    }
    // clang-format on
  };

  template <typename T, size_t N>
  struct array : array_impl<T, std::make_index_sequence<N>> {
    using array::array_impl::array_impl;
    T& operator[](size_t I) noexcept {
      return reinterpret_cast<T*>(voidify(*this))[I];
    }
    const T& operator[](size_t I) const noexcept {
      return reinterpret_cast<const T*>(voidifiy(*this))[I];
    }
    T* begin() noexcept {
      return reinterpret_cast<T*>(this);
    }
    T* end() noexcept {
      return reinterpret_cast<T*>(this) + N;
    }
    const T* begin() const noexcept {
      return reinterpret_cast<T*>(this);
    }
    const T* end() const noexcept {
      return reinterpret_cast<T*>(this) + N;
    }
  };

}  // namespace art
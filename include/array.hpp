#pragma once

#include <type_traits>

// implements array without C array, which means array in core C++ is useless
namespace art {

  template <typename T, size_t>
  struct value {
    T v;
  };

  template<typename,typename>
  struct array_impl;

  template <typename T, size_t... Is>
  struct array_impl<T, std::index_sequence<Is...>> : value<T, Is>... {};

  template <typename T, size_t N>
  struct array : array_impl<T, std::make_index_sequence<N>> {
    T& operator[](size_t I) noexcept {
      return *reinterpret_cast<T*>(reinterpret_cast<void*>(this));
    }
    const T& operator[](size_t I) const noexcept {
      return *reinterpret_cast<const T*>(const_cast<void*>(reinterpret_cast<const void*>(this)));
    }
  };

}  // namespace art
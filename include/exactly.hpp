#pragma once

#include <concepts>

// helper to check if type has EXACTLY required overload without conversions
// (but may be problem if resticted template overload wins in resolution..)
namespace art {
  template <typename T>
  struct exactly {
    template <std::same_as<std::remove_reference_t<T>> U>
    operator U&&();
  };
}  // namespace art

#pragma once

#include <optional>
#include <bit>

// unifies concept of NULL, things such as nullptr, nullopt, npos(nullsize_t?)
// nullany??(anyany if you knows...), possibly nullpoint/nullgeomobj in geometry
// O(-inf; -inf) for example
namespace art {

  template <typename T, typename... Ts>
  concept one_of = (std::same_as<T, Ts> || ...);

  struct null_t {
    // breakes compilation for every other type(such as void*, if it was operators nullptr_t/nullopt_t)
    template <one_of<std::nullptr_t, std::nullopt_t> T>
    consteval operator T() const noexcept {
      constexpr struct {} any_empty;
      return std::bit_cast<T>(any_empty);
    }
  };

  constexpr inline null_t null = {};

}  // namespace art
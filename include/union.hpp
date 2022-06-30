#pragma once

#include <type_traits>
#include <ranges>
#include <concepts>
#include <algorithm>
#include <array>

// implements union without union and with better interface.
// This means, that C++ must deprecate fcn union and remove a huge amount of garbage in the standard associated with it
namespace art {

  template <typename T>
  concept type = !std::is_union_v<std::remove_cvref_t<T>>;

  template<typename T, typename... Ts>
  concept any_of = (std::same_as<T, Ts> || ...);

  template <typename... Ts>
  struct union_t {
      // clang-bug now here(with ranges)
    alignas(std::ranges::max({alignof(Ts)...})) std::array<std::byte, std::ranges::max({sizeof(Ts)...})> buf;

    template<any_of<Ts...> U, typename... Args>
    [[nodiscard]] U& emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<U, Args&&...>) {
      return *std::construct_at(reinterpret_cast<U*>(&buf), std::forward<Args>(args)...);
    }
    template<any_of<Ts...> U>
    U& read_as() noexcept {
      return *reinterpret_cast<std::add_pointer_t<U>>(&buf);
    }
    template <any_of<Ts...> U>
    const U& read_as() const noexcept {
      return *reinterpret_cast<std::add_pointer_t<U>>(&buf);
    }
    template<any_of<Ts...> U>
    void destroy_as() const noexcept {
      std::destroy_at(reinterpret_cast<const U*>(reinterpret_cast<const std::byte*>(&buf)));
    }
  };

}  // namespace art
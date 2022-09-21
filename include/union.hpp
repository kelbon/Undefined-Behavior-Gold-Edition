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

  template <typename T, typename... Ts>
  concept any_of = (std::same_as<T, Ts> || ...);

  template <typename CRTP, typename T>
  struct ctor {
    // TODO consteval(internal compiler error now, 18-09-2022)
    constexpr ctor() = default;
    constexpr ctor(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>) {
      auto* p = static_cast<CRTP*>(this);
      std::construct_at(reinterpret_cast<T*>(p), std::move(value));
    }
    constexpr ctor(const T& value) noexcept(std::is_nothrow_copy_constructible_v<T>) {
      auto* p = static_cast<CRTP*>(this);
      std::construct_at(reinterpret_cast<T*>(p), value);
      on_debug(p->index = )
    }
  };
  template <typename T, typename...>
  struct first : std::type_identity<T> {};
#ifndef _NDEBUG
#define on_debug(...) __VA_ARGS__
#else
#define on_debug(...)
#endif
  template <typename... Ts>
  struct union_t : private ctor<union_t<Ts...>, Ts>... {
   private:
    // clang-bug now here(with ranges)
    alignas(std::ranges::max({alignof(Ts)...})) std::array<std::byte, std::ranges::max({sizeof(Ts)...})> buf;
    on_debug(std::size_t index;)
    using first_t = typename first<Ts...>::type;

   public:
    constexpr union_t() noexcept(std::is_nothrow_default_constructible_v<first_t>) requires(
        std::is_default_constructible_v<first_t>)
        on_debug(
            : index(0)) {
      std::construct_at(reinterpret_cast<first_t*>(buf.data()));
    }
    using ctor<union_t<Ts...>, Ts>::ctor...;
    template <any_of<Ts...> U, typename... Args>
    [[nodiscard]] U& emplace(Args&&... args) noexcept(std::is_nothrow_constructible_v<U, Args&&...>) {
      return *std::construct_at(reinterpret_cast<U*>(&buf), std::forward<Args>(args)...);
    }
    template <any_of<Ts...> U>
    U& read_as() noexcept {
      return *reinterpret_cast<std::add_pointer_t<U>>(&buf);
    }
    template <any_of<Ts...> U>
    const U& read_as() const noexcept {
      return *reinterpret_cast<std::add_pointer_t<U>>(&buf);
    }
    template <any_of<Ts...> U>
    void destroy_as() const noexcept {
      std::destroy_at(reinterpret_cast<const U*>(reinterpret_cast<const std::byte*>(&buf)));
    }
  };

}  // namespace art
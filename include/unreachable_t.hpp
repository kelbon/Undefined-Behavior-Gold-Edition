#pragma once

namespace art {

  struct [[maybe_unused]] unreachable_t final {
    // cant create it
    unreachable_t() = delete;
    // do it non trivial copy constructible(bit cast forbitten)
    unreachable_t(const unreachable_t&) noexcept {
    }
    // never called, must be without returns, but fcn msvc requires it
    template <typename T>
    [[noreturn]] constexpr operator T&() const noexcept {
      return *(T*)(void*)nullptr;
    }
    template <typename T>
    [[noreturn]] constexpr operator T&&() const noexcept {
      return std::move(*(T*)(void*)nullptr);
    }
  };

}  // namespace art
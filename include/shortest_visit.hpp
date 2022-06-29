#pragma once

#include <type_traits>
#include <variant>
#include <functional>

#define fwd(b) ::std::forward<decltype(b)>(b)
// clang-format off
// shortest possible realization of std::visit (+-)
namespace art {

  constexpr decltype(auto) visit(auto&& f, auto&& var) {
    return []<typename... Ts>(const std::variant<Ts...>&) consteval
    { return std::array{&+[](void* ptr, auto&& f_) { return fwd(f_)((Ts*)ptr); }...}; }(var)
    [var.index()](&var, fwd(f));
  }
  constexpr decltype(auto) visit(auto&& f, auto&& var, auto&&... vars) {
    return art::visit([&](auto&& v) { return art::visit(std::bind_front(fwd(f), fwd(v), fwd(vars)...)); }, fwd(var));
  }

}  // namespace art
// clang-format on
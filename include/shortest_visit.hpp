#pragma once

#include <type_traits>
#include <variant>
#include <functional>
#include <voidify.hpp>

#define fwd(b) ::std::forward<decltype(b)>(b)

// shortest possible realization of std::visit (+-)
namespace art {
  // okay it must be const / volatile/ const volatile versions,
  // but im lazy(it must be used with STTL::instance_of<std::variant> / types::extract + expand
  template <typename... Ts>
  constexpr decltype(auto) visit(auto&& f, std::variant<Ts...>& var) {
    return []() consteval {
      return std::array{+[](void* ptr, decltype(f)&& f_) -> decltype(auto) {
        return fwd(f_)(*std::get_if<Ts>(reinterpret_cast<std::variant<Ts...>*>(ptr)));
      }...};
    }
    ()[var.index()](voidify(var), fwd(f));
  }
  constexpr decltype(auto) visit(auto&& f, auto&& var, auto&&... vars) {
    return art::visit([&](auto&& v) { return art::visit(std::bind_front(fwd(f), fwd(v)), fwd(vars)...); },
                      fwd(var));
  }

}  // namespace art
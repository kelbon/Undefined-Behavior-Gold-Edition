#pragma once

#include <array>
#include <initializer_list>
#include <optional>
#include <type_traits>

// erases any function to signature void(void*) on compilation,
// This means C++ standard's ban cast function pointer to void* on constexpr is meaningless
namespace art {
  // for variable
  void* voidifiy(auto&& value) noexcept {
      return const_cast<void*>(reinterpret_cast<const volatile void*>(std::addressof(value)));
  }
  // for function
  template <auto* Foo, typename R, typename... Args>
  consteval auto voidify(R (*)(Args...)) noexcept {
    return [](void* ptr) -> void {
      return [](std::initializer_list<void*> p) -> void {
        [&]<size_t... Is>(std::index_sequence<Is...>) {
          auto it = p.begin();
          auto ret = *it;
          std::advance(it, 1);
          std::array erased_args{*std::next(it, Is)...};
          std::construct_at(
              reinterpret_cast<R*>(ret),
              Foo(static_cast<Args&&>(*reinterpret_cast<std::add_pointer_t<Args>>(erased_args[Is]))...));
        } // INVOKED HERE
        (std::index_sequence_for<Args...>{});
      }(*reinterpret_cast<std::initializer_list<void*>*>(ptr));
    };
  }
  // for function which returns void
  template <auto Functor, typename... Args>
  consteval auto voidify(void (*)(Args...)) noexcept {
    return [](void* ptr) -> void {
      return [](std::initializer_list<void*> p) -> void {
        [&]<size_t... Is>(std::index_sequence<Is...>) {
          auto it = p.begin();
          std::array erased_args{*std::next(it, Is)...};
          Functor(static_cast<Args&&>(*reinterpret_cast<std::add_pointer_t<Args>>(erased_args[Is]))...);
        }  // INVOKED HERE
        (std::index_sequence_for<Args...>{});
      }(*reinterpret_cast<std::initializer_list<void*>*>(ptr));
    };
  }
  // general case
  template<typename R, typename... Args>
  R call_voidified_as(R (*)(Args...), void (*erased_foo)(void*), std::type_identity_t<Args>... args) {
    using Ret = std::conditional_t<std::is_reference_v<R>, std::add_pointer_t<R>, R>;
    alignas(Ret) std::byte buf[sizeof(Ret)];
    std::initializer_list<void*> list{voidifiy(buf), voidifiy(args)...};
    erased_foo(&list);
    return std::move(*reinterpret_cast<Ret*>(buf));
  }
  // returns void
  template <typename... Args>
  void call_voidified_as(void (*)(Args...), void (*erased_foo)(void*), std::type_identity_t<Args>... args) {
    std::initializer_list<void*> list{voidifiy(args)...};
    erased_foo(&list);
  }
}  // namespace art

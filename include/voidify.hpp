#pragma once

#include <array>
#include <initializer_list>
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
    return +[](void* ptr) -> void {
      auto* p = reinterpret_cast<std::initializer_list<void*>*>(ptr);
      auto it = p->begin();
      auto* ret = reinterpret_cast<
          std::conditional_t<std::is_reference_v<R>, std::add_pointer_t<R>*, std::add_pointer_t<R>>>(*it);
      std::advance(it, 1);
      [&]<size_t... Is>(std::index_sequence<Is...>) {  // need for ordering in function call
        std::construct_at(
            ret,
            Foo(static_cast<Args&&>(*reinterpret_cast<std::add_pointer_t<Args>>(*std::next(it, Is)))...));
      }  // INVOKED HERE
      (std::index_sequence_for<Args...>{});
    };
  }
  // for function which returns void
  template <auto Foo, typename... Args>
  consteval auto voidify(void (*)(Args...)) noexcept {
    return [](void* ptr) -> void {
      auto* p = reinterpret_cast<std::initializer_list<void*>*>(ptr);
      auto it = p->begin();
      [&]<size_t... Is>(std::index_sequence<Is...>) {  // need for ordering in function call
        Foo(static_cast<Args&&>(*reinterpret_cast<std::add_pointer_t<Args>>(*std::next(it, Is)))...);
      }  // INVOKED HERE
      (std::index_sequence_for<Args...>{});
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

#pragma once
#ifdef CALL_FIZZ_BUZZ

#include <utility>

// returns result of fizz buzz in compiler error
namespace art {

  template <char... Values>
  struct c {};
  template <typename...>
  struct list {};

  enum : unsigned { fizz = 'fizz', buzz = 'buzz', fizzbuzz = fizz + buzz };

  template <std::size_t Value, char... Cs>
  consteval auto to_string(c<Cs...>) {
    if constexpr (!(Value % 15))
      return c<'f', 'i', 'z', 'z', 'b', 'u', 'z', 'z'>{};
    else if constexpr (!(Value % 5))
      return c<'f', 'i', 'z', 'z'>{};
    else if constexpr (!(Value % 3))
      return c<'b', 'u', 'z', 'z'>{};
    else if constexpr (Value < 10)
      return c<Cs..., '0' + Value>{};
    else
      return to_string<Value / 10>(c<Cs..., '0' + Value % 10>{});
  }

  int fizz_buzz() {
    return []<std::size_t... Is>(std::index_sequence<Is...>) {
      return list<decltype(to_string<Is>(c<>{}))...>{};
    }
    (std::make_index_sequence<100>{});
  }

}  // namespace art

#endif
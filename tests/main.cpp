

#include <shortest_visit.hpp>
#include <unreachable_t.hpp>
#include <voidify.hpp>
#include <union.hpp>
#include <array.hpp>
#include <maybe.hpp>
#include <exactly.hpp>

#include <fizzbuzz_compiler_error.hpp>
#include <iostream>

void foo(const std::variant<int, float, double> a, std::variant<char, bool, int> b) {
  std::visit([](auto&&... args) { (std::cout << ... << args); }, a, b);
}
[[noreturn]] art::unreachable_t my_exit(int i) noexcept {
  std::exit(i);
}
bool condition() {
  return true;
}
bool do_smth() {
  return false;
}
template <typename Exception, typename... Ts>
[[noreturn]] art::unreachable_t throw_(Ts&&... args) {
  throw Exception{std::forward<Ts>(args)...};
}
int foobar(double i, float j, int a) {
  std::cout << i << '\t' << j << '\t' << a << std::endl;
  return i;
}
void foobarvoid(std::string s) {
  std::cout << s << '\n';
}
struct check1 {
  static void foo(int, double);
  static void foo(int, float);
  static void foo(auto, double);
};

struct check2 {
  static void foo(int, float);
  static void foo(int, int);
};
template <typename T>
concept has = requires(art::exactly<int> v0, art::exactly<double> v1) {
  T::foo(v0, v1);
};
int main() {
  std::variant<int, float, double> d{5.};
  std::variant<int, float, double> dd{5};
  art::visit([](auto&&... x) { (std::cout << ... << std::is_const_v<std::remove_reference_t<decltype(x)>>); }, d, dd);
  static_assert(has<check1>);
  static_assert(!has<check2>);
#ifdef CALL_FIZZ_BUZZ
  art::fizz_buzz();
#endif
  true ? foobarvoid("") : std::terminate();
  int i = 0;
  art::maybe<int> mb = &i;
  art::maybe<const int> mb1 = &i;
  std::cout << *mb;
  std::cout << *mb1;
  std::string s = "hello world";
  art::maybe mbs = &s;
  art::maybe mbs1 = &std::as_const(s);
  mbs->resize(1);
  std::cout << mbs1->size() << '\n';
  constexpr auto erased_foo = art::voidify_fn<&foobar>(&foobar);
  auto res = art::call_voidified_as(&foobar, erased_foo, 143.5, 3.14f, 15);
  std::cout << res << '\n';
  constexpr auto erased_foo2 = art::voidify_fn<&foobarvoid>(&foobarvoid);
  art::union_t<int, float, std::string> u;
  (void)u.emplace<std::string>("union is useless");
  auto& x = u.read_as<std::string>();
  (void)x;
  u.destroy_as<std::string>();
  art::array<int, 10> arr{1, 2, 34};
  for (auto i : arr) {
    std::cout << i << '\n';
  }
  art::call_voidified_as(&foobarvoid, erased_foo2, "hello world");
  foo(5, 'c');
  condition() ? do_smth() : my_exit(-1);
  condition() ? do_smth() : throw_<std::bad_alloc>();
}
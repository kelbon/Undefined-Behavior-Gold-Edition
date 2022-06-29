

#include <shortest_visit.hpp>
#include <unreachable_t.hpp>
#include <voidify.hpp>
#include <union.hpp>
#include <array.hpp>

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
int foobar(double i, float j) {
  std::cout << i << '\t' << j << std::endl;
  return i;
}
void foobarvoid(std::string s) {
  std::cout << s << '\n';
}
int main() {
  constexpr auto erased_foo = art::voidify<&foobar>(&foobar);
  auto res = art::call_voidified_as(&foobar, erased_foo, 143.5, 3.14f);
  std::cout << res << '\n';
  constexpr auto erased_foo2 = art::voidify<&foobarvoid>(&foobarvoid);
  art::union_t<int, float, std::string> u;
  (void)u.emplace<std::string>("union is useless");
  auto& x =u.read_as<std::string>();
  u.destroy_as<std::string>();
  art::array<int, 10> arr{};
  art::call_voidified_as(&foobarvoid, erased_foo2, "hello world");
  foo(5, 'c');
  condition() ? do_smth() : my_exit(-1);
  condition() ? do_smth() : throw_<std::bad_alloc>();
}
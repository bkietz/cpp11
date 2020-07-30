#include <iostream>
#include "nameof.hpp"

enum class A { a, b, c, d };

int main() {
  std::cout << cpp11::nameof<A>() << std::endl;
  std::cout << cpp11::nameof<std::integral_constant<A, A::a>>() << std::endl;
  std::cout << cpp11::nameof<std::integral_constant<A, static_cast<A>(0)>>() << std::endl;
  std::cout << cpp11::nameof<std::integral_constant<A, static_cast<A>(1)>>() << std::endl;
  std::cout << cpp11::nameof<std::integral_constant<A, static_cast<A>(17)>>()
            << std::endl;
}

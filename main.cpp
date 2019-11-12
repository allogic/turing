#include <iostream>

#include "turing.hpp"

int main() {
  using namespace Turing;

  std::vector<uint8_t> program{
      LDX, 1,
      LDY, 2,
      ADD,
  };

  Ram ram{};
  Cpu{ram}.Run(program);
}
#include <array>
#include <cstdint>
#include <format>
#include <iostream>

#include "pic1650.hpp"

int main() {
  std::array<std::uint16_t, 512> opcodes;
  std::cin.read(reinterpret_cast<char *>(std::data(opcodes)),
                std::size(opcodes) * 2);
  pic1650::OpCodeStream os{std::cout};
  for (auto pc = 0uz; pc < std::size(opcodes); ++pc) {
    std::cout << std::format("{:<3d} x{:03X}:   ", pc, pc);
    os.dispatch(opcodes[pc]);
  }
}

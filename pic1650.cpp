#include <array>
#include <cstdint>
#include <iostream>

#include "pic1650.hpp"

int main() {
  std::array<std::uint16_t, 512> opcodes;
  std::cin.read(reinterpret_cast<char *>(std::data(opcodes)),
                std::size(opcodes) * 2);
  pic1650::LoudEmulator emulator{opcodes, std::cout};

  while (true) {
    emulator.tick();
  }
}

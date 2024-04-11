#pragma once

#include <bit>
#include <cassert>
#include <cstdint>
#include <format>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>

namespace pic1650 {

using OpCode = std::uint16_t;

class IOpCodes {
private:
  static constexpr bool is_match(const OpCode opcode,
                                 const std::uint16_t pattern,
                                 const std::uint16_t mask) {
    return (opcode & mask) == pattern;
  }

  static constexpr std::uint8_t f(const OpCode opcode) {
    return opcode & 0b1'1111;
  }

  static constexpr std::uint8_t d(const OpCode opcode) {
    return (opcode >> 5) & 0b1;
  }

  static constexpr std::uint8_t b(const OpCode opcode) {
    return (opcode >> 5) & 0b111;
  }

  static constexpr std::uint8_t k8(const OpCode opcode) {
    return opcode & 0b1111'1111;
  }

  static constexpr std::uint16_t k9(const OpCode opcode) {
    return opcode & 0b1'1111'1111;
  }

public:
  virtual void dispatch(const OpCode opcode) {
    if (is_match(opcode, 0b0000'0000'0000, 0b1111'1111'1111)) {
      NOP();
    } else if (is_match(opcode, 0b0000'0010'0000, 0b1111'1110'0000)) {
      MOVWF(f(opcode));
    } else if (is_match(opcode, 0b0000'0100'0000, 0b1111'1111'1111)) {
      CLRW();
    } else if (is_match(opcode, 0b0000'0110'0000, 0b1111'1110'0000)) {
      CLRF(f(opcode));
    } else if (is_match(opcode, 0b0000'1000'0000, 0b1111'1100'0000)) {
      SUBWF(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0000'1100'0000, 0b1111'1100'0000)) {
      DECF(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0001'0000'0000, 0b1111'1100'0000)) {
      IORWF(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0001'0100'0000, 0b1111'1100'0000)) {
      ANDWF(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0001'1000'0000, 0b1111'1100'0000)) {
      XORWF(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0001'1100'0000, 0b1111'1100'0000)) {
      ADDWF(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0010'0000'0000, 0b1111'1100'0000)) {
      MOVF(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0010'0100'0000, 0b1111'1100'0000)) {
      COMF(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0010'1000'0000, 0b1111'1100'0000)) {
      INCF(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0010'1100'0000, 0b1111'1100'0000)) {
      DECFSZ(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0011'0000'0000, 0b1111'1100'0000)) {
      RRF(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0011'0100'0000, 0b1111'1100'0000)) {
      RLF(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0011'1000'0000, 0b1111'1100'0000)) {
      SWAPF(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0011'1100'0000, 0b1111'1100'0000)) {
      INCFSZ(f(opcode), d(opcode));
    } else if (is_match(opcode, 0b0100'0000'0000, 0b1111'0000'0000)) {
      BCF(f(opcode), b(opcode));
    } else if (is_match(opcode, 0b0101'0000'0000, 0b1111'0000'0000)) {
      BSF(f(opcode), b(opcode));
    } else if (is_match(opcode, 0b0110'0000'0000, 0b1111'0000'0000)) {
      BTFSC(f(opcode), b(opcode));
    } else if (is_match(opcode, 0b0111'0000'0000, 0b1111'0000'0000)) {
      BTFSS(f(opcode), b(opcode));
    } else if (is_match(opcode, 0b1000'0000'0000, 0b1111'0000'0000)) {
      RETLW(k8(opcode));
    } else if (is_match(opcode, 0b1001'0000'0000, 0b1111'0000'0000)) {
      CALL(k8(opcode));
    } else if (is_match(opcode, 0b1010'0000'0000, 0b1110'0000'0000)) {
      GOTO(k9(opcode));
    } else if (is_match(opcode, 0b1100'0000'0000, 0b1111'0000'0000)) {
      MOVLW(k8(opcode));
    } else if (is_match(opcode, 0b1101'0000'0000, 0b1111'0000'0000)) {
      IORLW(k8(opcode));
    } else if (is_match(opcode, 0b1110'0000'0000, 0b1111'0000'0000)) {
      ANDLW(k8(opcode));
    } else if (is_match(opcode, 0b1111'0000'0000, 0b1111'0000'0000)) {
      XORLW(k8(opcode));
    } else {
      ILLEGAL_INSTRUCTION(opcode);
    }
  }

  virtual void NOP(){};
  virtual void MOVWF(const std::uint8_t f) = 0;
  virtual void CLRW() = 0;
  virtual void CLRF(const std::uint8_t f) = 0;
  virtual void SUBWF(const std::uint8_t f,
                     const std::uint8_t d) = 0;
  virtual void DECF(const std::uint8_t f,
                    const std::uint8_t d) = 0;
  virtual void IORWF(const std::uint8_t f,
                     const std::uint8_t d) = 0;
  virtual void ANDWF(const std::uint8_t f,
                     const std::uint8_t d) = 0;
  virtual void XORWF(const std::uint8_t f,
                     const std::uint8_t d) = 0;
  virtual void ADDWF(const std::uint8_t f,
                     const std::uint8_t d) = 0;
  virtual void MOVF(const std::uint8_t f,
                    const std::uint8_t d) = 0;
  virtual void COMF(const std::uint8_t f,
                    const std::uint8_t d) = 0;
  virtual void INCF(const std::uint8_t f,
                    const std::uint8_t d) = 0;
  virtual void DECFSZ(const std::uint8_t f,
                      const std::uint8_t d) = 0;
  virtual void RRF(const std::uint8_t f,
                   const std::uint8_t d) = 0;
  virtual void RLF(const std::uint8_t f,
                   const std::uint8_t d) = 0;
  virtual void SWAPF(const std::uint8_t f,
                     const std::uint8_t d) = 0;
  virtual void INCFSZ(const std::uint8_t f,
                      const std::uint8_t d) = 0;
  virtual void BCF(const std::uint8_t f,
                   const std::uint8_t b) = 0;
  virtual void BSF(const std::uint8_t f,
                   const std::uint8_t b) = 0;
  virtual void BTFSC(const std::uint8_t f,
                     const std::uint8_t b) = 0;
  virtual void BTFSS(const std::uint8_t f,
                     const std::uint8_t b) = 0;
  virtual void RETLW(const std::uint8_t k) = 0;
  virtual void CALL(const std::uint8_t k) = 0;
  virtual void GOTO(const std::uint16_t k) = 0;
  virtual void MOVLW(const std::uint8_t k) = 0;
  virtual void IORLW(const std::uint8_t k) = 0;
  virtual void ANDLW(const std::uint8_t k) = 0;
  virtual void XORLW(const std::uint8_t k) = 0;

  virtual void ILLEGAL_INSTRUCTION(const OpCode opcode) {
    throw std::runtime_error(
        std::format("b{:012b} is an illegal instruction", opcode));
  };
};

class OpCodeStream final : public IOpCodes {
private:
  std::ostream &os;

public:
  explicit OpCodeStream(std::ostream &os) noexcept : os{os} {}

  void NOP() override { os << std::format("{:8s}\n", "NOP"); };

  void MOVWF(const std::uint8_t f) override {
    os << std::format("{:8s} F{:<2d}\n", "MOVWF", f);
  }

  void CLRW() override { os << std::format("{:8s}\n", "CLRW"); }

  void CLRF(const std::uint8_t f) override {
    os << std::format("{:8s} F{:<2d}\n", "CLRF", f);
  }

  void SUBWF(const std::uint8_t f,
             const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "SUBWF", f, d);
  }

  void DECF(const std::uint8_t f, const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "DECF", f, d);
  }

  void IORWF(const std::uint8_t f,
             const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "IORWF", f, d);
  }

  void ANDWF(const std::uint8_t f,
             const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "ANDWF", f, d);
  }

  void XORWF(const std::uint8_t f,
             const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "XORWF", f, d);
  }

  void ADDWF(const std::uint8_t f,
             const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "ADDWF", f, d);
  }

  void MOVF(const std::uint8_t f, const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "MOVF", f, d);
  }

  void COMF(const std::uint8_t f, const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "COMF", f, d);
  }

  void INCF(const std::uint8_t f, const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "INCF", f, d);
  }

  void DECFSZ(const std::uint8_t f,
              const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "DECFSZ", f, d);
  }

  void RRF(const std::uint8_t f, const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "RRF", f, d);
  }

  void RLF(const std::uint8_t f, const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "RLF", f, d);
  }

  void SWAPF(const std::uint8_t f,
             const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "SWAPF", f, d);
  }

  void INCFSZ(const std::uint8_t f,
              const std::uint8_t d) override {
    os << std::format("{:8s} F{:<2d} D{:1d}\n", "INCFSZ", f, d);
  }

  void BCF(const std::uint8_t f, const std::uint8_t b) override {
    os << std::format("{:8s} F{:<2d} B{:1d}\n", "BCF", f, b);
  }

  void BSF(const std::uint8_t f, const std::uint8_t b) override {
    os << std::format("{:8s} F{:<2d} B{:1d}\n", "BSF", f, b);
  }

  void BTFSC(const std::uint8_t f,
             const std::uint8_t b) override {
    os << std::format("{:8s} F{:<2d} B{:1d}\n", "BTFSC", f, b);
  }

  void BTFSS(const std::uint8_t f,
             const std::uint8_t b) override {
    os << std::format("{:8s} F{:<2d} B{:1d}\n", "BTFSS", f, b);
  }

  void RETLW(const std::uint8_t k) override {
    os << std::format("{:8s} {:03d} x{:02X}   b{:08b} o{:03o}\n", "RETLW", k, k,
                      k, k);
  }

  void CALL(const std::uint8_t k) override {
    os << std::format("{:8s} {:<3d} x{:02X}   b{:08b} o{:03o}\n", "CALL", k, k,
                      k, k);
  }

  void GOTO(const std::uint16_t k) override {
    os << std::format("{:8s} {:<3d} x{:03X} b{:09b} o{:03o}\n", "GOTO", k, k, k,
                      k);
  }

  void MOVLW(const std::uint8_t k) override {
    os << std::format("{:8s} {:<3d} x{:02X}   b{:08b} o{:03o}\n", "MOVLW", k, k,
                      k, k);
  }

  void IORLW(const std::uint8_t k) override {
    os << std::format("{:8s} {:<3d} x{:02X}   b{:08b} o{:03o}\n", "IORLW", k, k,
                      k, k);
  }

  void ANDLW(const std::uint8_t k) override {
    os << std::format("{:8s} {:<3d} x{:02X}   b{:08b} o{:03o}\n", "ANDLW", k, k,
                      k, k);
  }

  void XORLW(const std::uint8_t k) override {
    os << std::format("{:8s} {:<3d} x{:02X}   b{:08b} o{:03o}\n", "XORLW", k, k,
                      k, k);
  }
};

class Emulator : public IOpCodes {
protected:
  const std::array<OpCode, 512> rom;
  std::uint16_t pc{0x1ffu};
  std::uint8_t rtcc{};
  std::uint8_t w{};
  std::uint8_t fsr{};
  struct Status {
    std::uint8_t C : 1;
    std::uint8_t DC : 1;
    std::uint8_t Z : 1;
    std::uint8_t reserved : 5;
  } status;
  std::array<std::uint8_t, 23> general_purpose_registers{};
  std::array<std::uint8_t, 4> inputs{0xffu, 0xffu, 0xffu, 0xffu};
  std::array<std::uint8_t, 4> output_latches{};
  std::array<std::uint16_t, 2> stack{0xffffu, 0xffffu};

  void increment_pc() { pc = (pc + 1u) & 0x1ffu; }

  std::uint8_t read_file(const std::uint8_t f) {
    assert(f < 32);
    switch (f) {
    case 0:
      // "0" is a virtual file which indirectly reads from the register file
      // pointed to by fsr
      assert(fsr != 0);
      return read_file(fsr);
    case 1:
      return rtcc;
    case 2:
      return pc & 0b1111'1111;
    case 3:
      return std::bit_cast<std::uint8_t>(status) & 0b111;
    case 4:
      return 0b1110'0000 | fsr;
    case 5:
    case 6:
    case 7:
    case 8:
      return read_io_port(f - 5u);
    default:
      return general_purpose_registers[f - 9];
    }
  }

  std::uint8_t write_file(const std::uint8_t f, const std::uint8_t x) {
    assert(f < 32);
    switch (f) {
    case 0:
      // "0" is a virtual file which indirectly writes to the register file
      // pointed to by fsr
      assert(fsr != 0);
      return write_file(fsr, x);
    case 1:
      rtcc = x;
      return x;
    case 2:
      pc = x;
      return x;
    case 3:
      status = std::bit_cast<Status>(static_cast<std::uint8_t>(x & 0b111u));
      return std::bit_cast<std::uint8_t>(status);
    case 4:
      fsr = x & 0b1'1111;
      return fsr;
    case 5:
    case 6:
    case 7:
    case 8:
      output_latches[f - 5] = x;
      return x;
    default:
      general_purpose_registers[f - 9] = x;
      return x;
    }
  }

  std::uint8_t write_file(const std::uint8_t f, const std::uint8_t d,
                          const std::uint8_t x) {
    if (d == 0u) {
      w = x;
      return x;
    } else {
      return write_file(f, x);
    }
  }

  virtual std::uint8_t read_io_port(const std::uint8_t port) {
    return output_latches[port] & inputs[port];
  }

public:
  Emulator(const std::array<OpCode, 512> &rom) : rom(rom) {}

  virtual void tick() {
    ++rtcc;
    const OpCode opcode = rom[pc];
    increment_pc();
    dispatch(opcode);
  }

  void input(const std::size_t port, const std::size_t bit, const bool set) {
    if (set) {
      input_high(port, bit);
    } else {
      input_low(port, bit);
    }
  }

  void input_high(const std::size_t port, const std::size_t bit) {
    const std::uint8_t bit_position = (1u << bit);
    inputs[port] = (inputs[port] & ~bit_position) | bit_position;
  }

  void input_low(const std::size_t port, const std::size_t bit) {
    const std::uint8_t bit_position = (1u << bit);
    inputs[port] = (inputs[port] & ~bit_position);
  }

  void NOP() override{};

  void MOVWF(const std::uint8_t f) override { write_file(f, w); }

  void CLRW() override {
    w = 0;
    status.Z = 0b1;
  }

  void CLRF(const std::uint8_t f) override {
    write_file(f, 0u);
    status.Z = 0b1;
  }

  void SUBWF(const std::uint8_t f,
             const std::uint8_t d) override {

    const auto value = read_file(f);
    status.C = (w <= value);
    status.DC = ((w & 0x0fu) <= (value & 0x0fu));
    const auto written = write_file(f, d, value - w);
    status.Z = (written == 0u);
  }

  void DECF(const std::uint8_t f, const std::uint8_t d) override {
    const auto value = read_file(f);
    const auto written = write_file(f, d, value - 1u);
    status.Z = (written == 0u);
  }

  void IORWF(const std::uint8_t f,
             const std::uint8_t d) override {
    const auto value = read_file(f);
    const auto written = write_file(f, d, value | w);
    status.Z = (written == 0u);
  }

  void ANDWF(const std::uint8_t f,
             const std::uint8_t d) override {
    const auto value = read_file(f);
    const auto written = write_file(f, d, value & w);
    status.Z = (written == 0u);
  }

  void XORWF(const std::uint8_t f,
             const std::uint8_t d) override {
    const auto value = read_file(f);
    const auto written = write_file(f, d, value ^ w);
    status.Z = (written == 0u);
  }

  void ADDWF(const std::uint8_t f,
             const std::uint8_t d) override {
    const auto value = read_file(f);
    status.C = ((std::uint16_t{w} + std::uint16_t{value}) > 0xffu);
    status.DC = (((w & 0x0fu) + (value & 0x0fu)) > 0x0fu);
    const auto written = write_file(f, d, value + w);
    status.Z = (written == 0u);
  }

  void MOVF(const std::uint8_t f, const std::uint8_t d) override {
    const auto value = read_file(f);
    const auto written = write_file(f, d, value);
    status.Z = (written == 0u);
  }

  void COMF(const std::uint8_t f, const std::uint8_t d) override {
    const auto value = read_file(f);
    const auto written = write_file(f, d, ~value);
    status.Z = (written == 0u);
  }

  void INCF(const std::uint8_t f, const std::uint8_t d) override {
    const auto value = read_file(f);
    const auto written = write_file(f, d, value + 1u);
    status.Z = (written == 0u);
  }

  void DECFSZ(const std::uint8_t f,
              const std::uint8_t d) override {
    const auto value = read_file(f);
    const auto written = write_file(f, d, value - 1u);
    if (written == 0u) {
      increment_pc();
    }
  }

  void RRF(const std::uint8_t f, const std::uint8_t d) override {
    const auto value = read_file(f);
    status.C = value & 0b1;
    write_file(f, d, (value >> 1) | (status.C << 7));
  }

  void RLF(const std::uint8_t f, const std::uint8_t d) override {
    const auto value = read_file(f);
    status.C = value >> 7;
    write_file(f, d, (value << 1) | status.C);
  }

  void SWAPF(const std::uint8_t f,
             const std::uint8_t d) override {
    const auto value = read_file(f);
    write_file(f, d, (value << 4) | (value >> 4));
  }

  void INCFSZ(const std::uint8_t f,
              const std::uint8_t d) override {
    const auto value = read_file(f);
    const auto written = write_file(f, d, value + 1u);
    if (written == 0u) {
      increment_pc();
    }
  }

  void BCF(const std::uint8_t f, const std::uint8_t b) override {
    const auto value = read_file(f);
    write_file(f, value & ~(0b1 << b));
  }

  void BSF(const std::uint8_t f, const std::uint8_t b) override {
    const auto value = read_file(f);
    write_file(f, value | (0b1 << b));
  }

  void BTFSC(const std::uint8_t f,
             const std::uint8_t b) override {
    const auto value = read_file(f);
    if (0u == (value & (0b1 << b))) {
      increment_pc();
    }
  }

  void BTFSS(const std::uint8_t f,
             const std::uint8_t b) override {
    const auto value = read_file(f);
    if (0u != (value & (0b1 << b))) {
      increment_pc();
    }
  }

  void RETLW(const std::uint8_t k) override {
    w = k;
    pc = stack[0];
    stack[0] = stack[1];
    stack[1] = 0xffffu;
  }

  void CALL(const std::uint8_t k) override {
    stack[1] = stack[0];
    stack[0] = pc & 0x1ffu;
    pc = k;
  }

  void GOTO(const std::uint16_t k) override { pc = k; }

  void MOVLW(const std::uint8_t k) override { w = k; }

  void IORLW(const std::uint8_t k) override {
    w = w | k;
    status.Z = (w == 0u);
  }

  void ANDLW(const std::uint8_t k) override {
    w = w & k;
    status.Z = (w == 0u);
  }

  void XORLW(const std::uint8_t k) override {
    w = w ^ k;
    status.Z = (w == 0u);
  }

  std::uint16_t PC() const { return pc; }

  auto a() const { return output_latches[0]; }
  auto b() const { return output_latches[1]; }
  auto c() const { return output_latches[2]; }
  auto d() const { return output_latches[3]; }
};

class LoudEmulator final : public Emulator {
private:
  std::stringstream ss{};
  OpCodeStream opcode_stream;
  std::ostream &os;

public:
  std::uint64_t cnt{};

  LoudEmulator(const std::array<OpCode, 512> &rom, std::ostream &os)
      : Emulator{rom}, opcode_stream{ss}, os{os} {
    os << "cnt,starting_pc,rtcc,pc,C,DC,Z,fsr,w,RA,RB,RC,RD,f9,f10,f11,"
          "r12,f13,f14,f15,f16,f17,f18,f19,f20,f21,f22,f23,f24,f25,f26,f27,f28,"
          "r29,f30,f31,stack0,stack1,opcode,decoded"
       << std::endl;
  }

  void tick() override {
    const auto starting_pc = pc;
    const auto opcode = rom[starting_pc];
    opcode_stream.dispatch(opcode);
    Emulator::tick();
    os << std::format("{:d},{:d},{:d},{:d},{:d},{:d},{:d},{:d},{:d},", cnt++,
                      starting_pc, rtcc, pc, +status.C, +status.DC, +status.Z,
                      fsr, w);
    for (const auto x : output_latches) {
      os << std::format("0b{:08b},", x);
    }
    for (const auto x : general_purpose_registers) {
      os << std::format("{:d},", x);
    }
    os << std::format("{:d},{:d},0b{:012b},{:s}", stack[0], stack[1], opcode,
                      ss.str())
       << std::flush;
    ss.str("");
  }
};

} // namespace pic1650
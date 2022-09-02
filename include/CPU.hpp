#ifndef __CPU_H__
#define __CPU_H__

/**
 * @brief Contains 6502 CPU Emulation
 *
 */

#include "Bus.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <vector>


class Bus;

class CPU final {

public:
  CPU() = default;
  ~CPU() = default;

  auto connect(Bus *bus) -> void;
  auto read(uint16_t addr) -> uint8_t;
  auto write(uint16_t addr, uint8_t data) -> void;

  // addressing modes
  auto IMP() -> uint8_t;
  auto ZP0() -> uint8_t;
  auto ZPY() -> uint8_t;
  auto ABS() -> uint8_t;
  auto ABY() -> uint8_t;
  auto IZX() -> uint8_t;
  auto IMM() -> uint8_t;
  auto ZPX() -> uint8_t;
  auto REL() -> uint8_t;
  auto ABX() -> uint8_t;
  auto IND() -> uint8_t;
  auto IZY() -> uint8_t;

  // legal opcodes defined in the NES
  auto ADC() -> uint8_t;
  auto AND() -> uint8_t;
  auto ASL() -> uint8_t;
  auto BCC() -> uint8_t;
  auto BCS() -> uint8_t;
  auto BEQ() -> uint8_t;
  auto BIT() -> uint8_t;
  auto BMI() -> uint8_t;
  auto BNE() -> uint8_t;
  auto BPL() -> uint8_t;
  auto BRK() -> uint8_t;
  auto BVC() -> uint8_t;
  auto BVS() -> uint8_t;
  auto CLC() -> uint8_t;
  auto CLD() -> uint8_t;
  auto CLI() -> uint8_t;
  auto CLV() -> uint8_t;
  auto CMP() -> uint8_t;
  auto CPX() -> uint8_t;
  auto CPY() -> uint8_t;
  auto DEC() -> uint8_t;
  auto DEX() -> uint8_t;
  auto DEY() -> uint8_t;
  auto EOR() -> uint8_t;
  auto INC() -> uint8_t;
  auto INX() -> uint8_t;
  auto INY() -> uint8_t;
  auto JMP() -> uint8_t;
  auto JSR() -> uint8_t;
  auto LDA() -> uint8_t;
  auto LDX() -> uint8_t;
  auto LDY() -> uint8_t;
  auto LSR() -> uint8_t;
  auto NOP() -> uint8_t;
  auto ORA() -> uint8_t;
  auto PHA() -> uint8_t;
  auto PHP() -> uint8_t;
  auto PLA() -> uint8_t;
  auto PLP() -> uint8_t;
  auto ROL() -> uint8_t;
  auto ROR() -> uint8_t;
  auto RTI() -> uint8_t;
  auto RTS() -> uint8_t;
  auto SBC() -> uint8_t;
  auto SEC() -> uint8_t;
  auto SED() -> uint8_t;
  auto SEI() -> uint8_t;
  auto STA() -> uint8_t;
  auto STX() -> uint8_t;
  auto STY() -> uint8_t;
  auto TAX() -> uint8_t;
  auto TAY() -> uint8_t;
  auto TSX() -> uint8_t;
  auto TXA() -> uint8_t;
  auto TXS() -> uint8_t;
  auto TYA() -> uint8_t;

  // handling illegal opcodes
  auto XXX() -> uint8_t;

  // input signals
  auto clock() -> void;
  auto reset() -> void; // handling reset signal
  auto irq() -> void;   // interrupt request
  auto nmi() -> void;   // non-maskable interrupt

  auto fetch() -> uint8_t;
  auto is_complete() -> bool;
  auto disassemble(uint16_t start, uint16_t stop)
      -> std::map<uint16_t, std::string>;

public:
  enum class Flags : uint8_t {
    C = (1 << 0), // Carry Bit
    Z = (1 << 1), // Zero
    I = (1 << 2), // Interrupt Disable
    D = (1 << 3), // Decimal Mode (unused in this implementation)
    B = (1 << 4), // Break
    U = (1 << 5), // Unused
    V = (1 << 6), // Overflow
    N = (1 << 7), // Negative
  };

  uint8_t a = 0x00;      // Accumulator Register
  uint8_t x = 0x00;      // X Register
  uint8_t y = 0x00;      // Y Register
  uint8_t stkp = 0x00;   // Stack Pointer (points to location on bus)
  uint16_t pc = 0x0000;  // Program Counter
  uint8_t status = 0x00; // Status Register

public:
  friend auto operator|(CPU::Flags lhs, CPU::Flags rhs) -> uint8_t;
  friend auto operator|(uint8_t lhs, CPU::Flags rhs) -> uint8_t;
  friend auto operator&(CPU::Flags lhs, CPU::Flags rhs) -> uint8_t;
  friend auto operator&(uint8_t lhs, CPU::Flags rhs) -> uint8_t;
  friend auto operator^(CPU::Flags lhs, CPU::Flags rhs) -> uint8_t;
  friend auto operator~(CPU::Flags rhs) -> uint8_t;
  friend auto operator|=(uint8_t &lhs, CPU::Flags rhs) -> void;

private:
  auto get_flag(CPU::Flags flag) -> uint8_t;
  auto set_flag(CPU::Flags flag, bool value) -> void;

private:
  uint8_t fetched = 0x00;     // for storing the fetched data
  uint16_t addr_abs = 0x0000; // for storing different locations in memory based
                              // on the addressing mode
  uint16_t addr_rel = 0x00;   // for storing info to jump into relative address
  uint8_t opcode = 0x00; // for storing the opcode that is currently executing
  uint8_t cycles = 0;    // for storing the cycles for the instruction execution

  uint16_t temp = 0x0000;
  uint32_t clock_count = 0;

  struct Instruction final {
    std::string name;
    uint8_t (CPU::*operate)(void) = nullptr;
    uint8_t (CPU::*address_mode)(void) = nullptr;
    uint8_t cycles{};
  };

  // mapped instructions as specified in
  // http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf
  const std::vector<Instruction> lookup{
      {"BRK", &CPU::BRK, &CPU::IMM, 7}, {"ORA", &CPU::ORA, &CPU::IZX, 6},
      {"???", &CPU::XXX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 8},
      {"???", &CPU::NOP, &CPU::IMP, 3}, {"ORA", &CPU::ORA, &CPU::ZP0, 3},
      {"ASL", &CPU::ASL, &CPU::ZP0, 5}, {"???", &CPU::XXX, &CPU::IMP, 5},
      {"PHP", &CPU::PHP, &CPU::IMP, 3}, {"ORA", &CPU::ORA, &CPU::IMM, 2},
      {"ASL", &CPU::ASL, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 2},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"ORA", &CPU::ORA, &CPU::ABS, 4},
      {"ASL", &CPU::ASL, &CPU::ABS, 6}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"BPL", &CPU::BPL, &CPU::REL, 2}, {"ORA", &CPU::ORA, &CPU::IZY, 5},
      {"???", &CPU::XXX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 8},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"ORA", &CPU::ORA, &CPU::ZPX, 4},
      {"ASL", &CPU::ASL, &CPU::ZPX, 6}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"CLC", &CPU::CLC, &CPU::IMP, 2}, {"ORA", &CPU::ORA, &CPU::ABY, 4},
      {"???", &CPU::NOP, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 7},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"ORA", &CPU::ORA, &CPU::ABX, 4},
      {"ASL", &CPU::ASL, &CPU::ABX, 7}, {"???", &CPU::XXX, &CPU::IMP, 7},
      {"JSR", &CPU::JSR, &CPU::ABS, 6}, {"AND", &CPU::AND, &CPU::IZX, 6},
      {"???", &CPU::XXX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 8},
      {"BIT", &CPU::BIT, &CPU::ZP0, 3}, {"AND", &CPU::AND, &CPU::ZP0, 3},
      {"ROL", &CPU::ROL, &CPU::ZP0, 5}, {"???", &CPU::XXX, &CPU::IMP, 5},
      {"PLP", &CPU::PLP, &CPU::IMP, 4}, {"AND", &CPU::AND, &CPU::IMM, 2},
      {"ROL", &CPU::ROL, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 2},
      {"BIT", &CPU::BIT, &CPU::ABS, 4}, {"AND", &CPU::AND, &CPU::ABS, 4},
      {"ROL", &CPU::ROL, &CPU::ABS, 6}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"BMI", &CPU::BMI, &CPU::REL, 2}, {"AND", &CPU::AND, &CPU::IZY, 5},
      {"???", &CPU::XXX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 8},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"AND", &CPU::AND, &CPU::ZPX, 4},
      {"ROL", &CPU::ROL, &CPU::ZPX, 6}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"SEC", &CPU::SEC, &CPU::IMP, 2}, {"AND", &CPU::AND, &CPU::ABY, 4},
      {"???", &CPU::NOP, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 7},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"AND", &CPU::AND, &CPU::ABX, 4},
      {"ROL", &CPU::ROL, &CPU::ABX, 7}, {"???", &CPU::XXX, &CPU::IMP, 7},
      {"RTI", &CPU::RTI, &CPU::IMP, 6}, {"EOR", &CPU::EOR, &CPU::IZX, 6},
      {"???", &CPU::XXX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 8},
      {"???", &CPU::NOP, &CPU::IMP, 3}, {"EOR", &CPU::EOR, &CPU::ZP0, 3},
      {"LSR", &CPU::LSR, &CPU::ZP0, 5}, {"???", &CPU::XXX, &CPU::IMP, 5},
      {"PHA", &CPU::PHA, &CPU::IMP, 3}, {"EOR", &CPU::EOR, &CPU::IMM, 2},
      {"LSR", &CPU::LSR, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 2},
      {"JMP", &CPU::JMP, &CPU::ABS, 3}, {"EOR", &CPU::EOR, &CPU::ABS, 4},
      {"LSR", &CPU::LSR, &CPU::ABS, 6}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"BVC", &CPU::BVC, &CPU::REL, 2}, {"EOR", &CPU::EOR, &CPU::IZY, 5},
      {"???", &CPU::XXX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 8},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"EOR", &CPU::EOR, &CPU::ZPX, 4},
      {"LSR", &CPU::LSR, &CPU::ZPX, 6}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"CLI", &CPU::CLI, &CPU::IMP, 2}, {"EOR", &CPU::EOR, &CPU::ABY, 4},
      {"???", &CPU::NOP, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 7},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"EOR", &CPU::EOR, &CPU::ABX, 4},
      {"LSR", &CPU::LSR, &CPU::ABX, 7}, {"???", &CPU::XXX, &CPU::IMP, 7},
      {"RTS", &CPU::RTS, &CPU::IMP, 6}, {"ADC", &CPU::ADC, &CPU::IZX, 6},
      {"???", &CPU::XXX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 8},
      {"???", &CPU::NOP, &CPU::IMP, 3}, {"ADC", &CPU::ADC, &CPU::ZP0, 3},
      {"ROR", &CPU::ROR, &CPU::ZP0, 5}, {"???", &CPU::XXX, &CPU::IMP, 5},
      {"PLA", &CPU::PLA, &CPU::IMP, 4}, {"ADC", &CPU::ADC, &CPU::IMM, 2},
      {"ROR", &CPU::ROR, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 2},
      {"JMP", &CPU::JMP, &CPU::IND, 5}, {"ADC", &CPU::ADC, &CPU::ABS, 4},
      {"ROR", &CPU::ROR, &CPU::ABS, 6}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"BVS", &CPU::BVS, &CPU::REL, 2}, {"ADC", &CPU::ADC, &CPU::IZY, 5},
      {"???", &CPU::XXX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 8},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"ADC", &CPU::ADC, &CPU::ZPX, 4},
      {"ROR", &CPU::ROR, &CPU::ZPX, 6}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"SEI", &CPU::SEI, &CPU::IMP, 2}, {"ADC", &CPU::ADC, &CPU::ABY, 4},
      {"???", &CPU::NOP, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 7},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"ADC", &CPU::ADC, &CPU::ABX, 4},
      {"ROR", &CPU::ROR, &CPU::ABX, 7}, {"???", &CPU::XXX, &CPU::IMP, 7},
      {"???", &CPU::NOP, &CPU::IMP, 2}, {"STA", &CPU::STA, &CPU::IZX, 6},
      {"???", &CPU::NOP, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"STY", &CPU::STY, &CPU::ZP0, 3}, {"STA", &CPU::STA, &CPU::ZP0, 3},
      {"STX", &CPU::STX, &CPU::ZP0, 3}, {"???", &CPU::XXX, &CPU::IMP, 3},
      {"DEY", &CPU::DEY, &CPU::IMP, 2}, {"???", &CPU::NOP, &CPU::IMP, 2},
      {"TXA", &CPU::TXA, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 2},
      {"STY", &CPU::STY, &CPU::ABS, 4}, {"STA", &CPU::STA, &CPU::ABS, 4},
      {"STX", &CPU::STX, &CPU::ABS, 4}, {"???", &CPU::XXX, &CPU::IMP, 4},
      {"BCC", &CPU::BCC, &CPU::REL, 2}, {"STA", &CPU::STA, &CPU::IZY, 6},
      {"???", &CPU::XXX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"STY", &CPU::STY, &CPU::ZPX, 4}, {"STA", &CPU::STA, &CPU::ZPX, 4},
      {"STX", &CPU::STX, &CPU::ZPY, 4}, {"???", &CPU::XXX, &CPU::IMP, 4},
      {"TYA", &CPU::TYA, &CPU::IMP, 2}, {"STA", &CPU::STA, &CPU::ABY, 5},
      {"TXS", &CPU::TXS, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 5},
      {"???", &CPU::NOP, &CPU::IMP, 5}, {"STA", &CPU::STA, &CPU::ABX, 5},
      {"???", &CPU::XXX, &CPU::IMP, 5}, {"???", &CPU::XXX, &CPU::IMP, 5},
      {"LDY", &CPU::LDY, &CPU::IMM, 2}, {"LDA", &CPU::LDA, &CPU::IZX, 6},
      {"LDX", &CPU::LDX, &CPU::IMM, 2}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"LDY", &CPU::LDY, &CPU::ZP0, 3}, {"LDA", &CPU::LDA, &CPU::ZP0, 3},
      {"LDX", &CPU::LDX, &CPU::ZP0, 3}, {"???", &CPU::XXX, &CPU::IMP, 3},
      {"TAY", &CPU::TAY, &CPU::IMP, 2}, {"LDA", &CPU::LDA, &CPU::IMM, 2},
      {"TAX", &CPU::TAX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 2},
      {"LDY", &CPU::LDY, &CPU::ABS, 4}, {"LDA", &CPU::LDA, &CPU::ABS, 4},
      {"LDX", &CPU::LDX, &CPU::ABS, 4}, {"???", &CPU::XXX, &CPU::IMP, 4},
      {"BCS", &CPU::BCS, &CPU::REL, 2}, {"LDA", &CPU::LDA, &CPU::IZY, 5},
      {"???", &CPU::XXX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 5},
      {"LDY", &CPU::LDY, &CPU::ZPX, 4}, {"LDA", &CPU::LDA, &CPU::ZPX, 4},
      {"LDX", &CPU::LDX, &CPU::ZPY, 4}, {"???", &CPU::XXX, &CPU::IMP, 4},
      {"CLV", &CPU::CLV, &CPU::IMP, 2}, {"LDA", &CPU::LDA, &CPU::ABY, 4},
      {"TSX", &CPU::TSX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 4},
      {"LDY", &CPU::LDY, &CPU::ABX, 4}, {"LDA", &CPU::LDA, &CPU::ABX, 4},
      {"LDX", &CPU::LDX, &CPU::ABY, 4}, {"???", &CPU::XXX, &CPU::IMP, 4},
      {"CPY", &CPU::CPY, &CPU::IMM, 2}, {"CMP", &CPU::CMP, &CPU::IZX, 6},
      {"???", &CPU::NOP, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 8},
      {"CPY", &CPU::CPY, &CPU::ZP0, 3}, {"CMP", &CPU::CMP, &CPU::ZP0, 3},
      {"DEC", &CPU::DEC, &CPU::ZP0, 5}, {"???", &CPU::XXX, &CPU::IMP, 5},
      {"INY", &CPU::INY, &CPU::IMP, 2}, {"CMP", &CPU::CMP, &CPU::IMM, 2},
      {"DEX", &CPU::DEX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 2},
      {"CPY", &CPU::CPY, &CPU::ABS, 4}, {"CMP", &CPU::CMP, &CPU::ABS, 4},
      {"DEC", &CPU::DEC, &CPU::ABS, 6}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"BNE", &CPU::BNE, &CPU::REL, 2}, {"CMP", &CPU::CMP, &CPU::IZY, 5},
      {"???", &CPU::XXX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 8},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"CMP", &CPU::CMP, &CPU::ZPX, 4},
      {"DEC", &CPU::DEC, &CPU::ZPX, 6}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"CLD", &CPU::CLD, &CPU::IMP, 2}, {"CMP", &CPU::CMP, &CPU::ABY, 4},
      {"NOP", &CPU::NOP, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 7},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"CMP", &CPU::CMP, &CPU::ABX, 4},
      {"DEC", &CPU::DEC, &CPU::ABX, 7}, {"???", &CPU::XXX, &CPU::IMP, 7},
      {"CPX", &CPU::CPX, &CPU::IMM, 2}, {"SBC", &CPU::SBC, &CPU::IZX, 6},
      {"???", &CPU::NOP, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 8},
      {"CPX", &CPU::CPX, &CPU::ZP0, 3}, {"SBC", &CPU::SBC, &CPU::ZP0, 3},
      {"INC", &CPU::INC, &CPU::ZP0, 5}, {"???", &CPU::XXX, &CPU::IMP, 5},
      {"INX", &CPU::INX, &CPU::IMP, 2}, {"SBC", &CPU::SBC, &CPU::IMM, 2},
      {"NOP", &CPU::NOP, &CPU::IMP, 2}, {"???", &CPU::SBC, &CPU::IMP, 2},
      {"CPX", &CPU::CPX, &CPU::ABS, 4}, {"SBC", &CPU::SBC, &CPU::ABS, 4},
      {"INC", &CPU::INC, &CPU::ABS, 6}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"BEQ", &CPU::BEQ, &CPU::REL, 2}, {"SBC", &CPU::SBC, &CPU::IZY, 5},
      {"???", &CPU::XXX, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 8},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"SBC", &CPU::SBC, &CPU::ZPX, 4},
      {"INC", &CPU::INC, &CPU::ZPX, 6}, {"???", &CPU::XXX, &CPU::IMP, 6},
      {"SED", &CPU::SED, &CPU::IMP, 2}, {"SBC", &CPU::SBC, &CPU::ABY, 4},
      {"NOP", &CPU::NOP, &CPU::IMP, 2}, {"???", &CPU::XXX, &CPU::IMP, 7},
      {"???", &CPU::NOP, &CPU::IMP, 4}, {"SBC", &CPU::SBC, &CPU::ABX, 4},
      {"INC", &CPU::INC, &CPU::ABX, 7}, {"???", &CPU::XXX, &CPU::IMP, 7},
  };

  Bus *m_bus{nullptr};
};

#endif // __CPU_H__
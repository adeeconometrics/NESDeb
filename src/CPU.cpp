#include "../include/CPU.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <type_traits>

auto CPU::connect(Bus *bus) -> void { m_bus = bus; }

auto CPU::read(uint16_t addr) -> uint8_t { return m_bus->read_cpu(addr); }

auto CPU::write(uint16_t addr, uint8_t data) -> void {
  m_bus->write_cpu(addr, data);
}

auto CPU::get_flag(CPU::Flags flag) -> uint8_t {
  return ((status & flag) > 0) ? 1 : 0;
}

auto CPU::set_flag(CPU::Flags flag, bool value) -> void {
  if (value) {
    status |= flag;
  } else {
    status &= ~flag;
  }
}

// Instruction Operator Overloads
auto operator|(CPU::Flags lhs, CPU::Flags rhs) -> uint8_t {
  return static_cast<uint8_t>(
      static_cast<std::underlying_type_t<CPU::Flags>>(lhs) |
      static_cast<std::underlying_type_t<CPU::Flags>>(rhs));
}

auto operator|(uint8_t lhs, CPU::Flags rhs) -> uint8_t {
  return lhs | static_cast<uint8_t>(
                   static_cast<std::underlying_type_t<CPU::Flags>>(rhs));
}

auto operator|=(uint8_t &lhs, CPU::Flags rhs) -> void {
  lhs = lhs | static_cast<uint8_t>(
                  static_cast<std::underlying_type_t<CPU::Flags>>(rhs));
}

auto operator&(CPU::Flags lhs, CPU::Flags rhs) -> uint8_t {
  return static_cast<uint8_t>(
      static_cast<std::underlying_type_t<CPU::Flags>>(lhs) &
      static_cast<std::underlying_type_t<CPU::Flags>>(rhs));
}

auto operator&(uint8_t lhs, CPU::Flags rhs) -> uint8_t {
  return lhs & static_cast<uint8_t>(
                   static_cast<std::underlying_type_t<CPU::Flags>>(rhs));
}

auto operator^(CPU::Flags lhs, CPU::Flags rhs) -> uint8_t {
  return static_cast<uint8_t>(
      static_cast<std::underlying_type_t<CPU::Flags>>(lhs) ^
      static_cast<std::underlying_type_t<CPU::Flags>>(rhs));
}

auto operator~(CPU::Flags rhs) -> uint8_t {
  return ~static_cast<uint8_t>(
      static_cast<std::underlying_type_t<CPU::Flags>>(rhs));
}

// addressing modes
auto CPU::IMP() -> uint8_t {
  fetched = a;
  return 0;
}

auto CPU::ZP0() -> uint8_t {
  addr_abs = read(pc);
  pc++;
  addr_abs &= 0x00ff;

  return 0;
}

auto CPU::ZPY() -> uint8_t {
  addr_abs = (read(pc) + y);
  pc++;
  addr_abs &= 0x0ff;

  return 0;
}

auto CPU::ABS() -> uint8_t {
  uint16_t low_byte = read(pc);
  pc++;
  uint16_t high_byte = read(pc);
  pc++;

  addr_abs = (high_byte << 8) | low_byte;

  return 0;
}

auto CPU::ABY() -> uint8_t {
  uint16_t low_byte = read(pc);
  pc++;
  uint16_t high_byte = read(pc);
  pc++;

  addr_abs = (high_byte << 8) | low_byte;
  addr_abs += y;

  if ((addr_abs & 0xff00) != (high_byte << 8))
    return 1;
  return 0;
}

auto CPU::IZX() -> uint8_t {
  uint16_t t = read(pc);
  pc++;

  uint16_t low = read((uint16_t)(t + (uint16_t)x) & 0x00ff);
  uint16_t high = read((uint16_t)(t + (uint16_t)x + 1) & 0x00ff);

  addr_abs = (high << 8) | low;

  return 0;
}

auto CPU::IMM() -> uint8_t {
  addr_abs = pc++;
  return 0;
}

auto CPU::ZPX() -> uint8_t {
  addr_abs = (read(pc) + x);
  pc++;
  addr_abs &= 0x0ff;

  return 0;
}

auto CPU::REL() -> uint8_t {
  addr_rel = read(pc);
  pc++;

  if (addr_rel & 0x80)  // checking for signed numbers
    addr_rel |= 0xff00; // set high byte to all 1's
  return 0;
}

auto CPU::ABX() -> uint8_t {
  uint16_t low_byte = read(pc);
  pc++;
  uint16_t high_byte = read(pc);
  pc++;

  addr_abs = (high_byte << 8) | low_byte;
  addr_abs += x;

  if ((addr_abs & 0xff00) != (high_byte << 8))
    return 1;
  return 0;
}

auto CPU::IND() -> uint8_t {
  uint16_t ptr_low = read(pc);
  pc++;
  uint16_t ptr_high = read(pc);
  pc++;

  uint16_t ptr = (ptr_high << 8) | ptr_low;

  if (ptr_low == 0x00ff) { // simulation of page boundary hardware bug
    addr_abs = (read(ptr & 0xff00) << 8) | read(ptr + 0);
  } else {
    addr_abs = (read(ptr + 1) << 8) | read(ptr + 0);
  }

  return 0;
}

auto CPU::IZY() -> uint8_t {
  uint16_t t = read(pc);
  pc++;

  uint16_t low = read(t & 0x00ff);
  uint16_t high = read((t + 1) & 0x00ff);

  addr_abs = (high << 8) | low;
  addr_abs += y;

  if ((addr_abs & 0xff00) != (high << 8))
    return 1;
  return 0;
}

// legal opcodes defined in the NES

auto CPU::ADC() -> uint8_t {
  fetch();

  uint16_t temp =
      (uint16_t)a + (uint16_t)fetched + (uint16_t)get_flag(Flags::C);
  set_flag(Flags::C, temp > 255);
  set_flag(Flags::Z, (temp & 0x00ff) == 0);
  set_flag(Flags::N, temp & 0x80);
  set_flag(Flags::V, (~((uint16_t)a ^ (uint16_t)fetched) &
                      ((uint16_t)a ^ (uint16_t)temp)) &
                         0x0080);
  set_flag(Flags::N, temp & 0x80);

  a = temp & 0x00ff;

  return 1;
}

auto CPU::AND() -> uint8_t {
  fetch();

  a &= fetched;
  set_flag(Flags::Z, a == 0x00);
  set_flag(Flags::N, a & 0x80);

  return 1;
}

auto CPU::ASL() -> uint8_t {
  fetch();
  temp = (uint16_t)fetched << 1;

  set_flag(Flags::C, (temp & 0xff00) > 0);
  set_flag(Flags::Z, (temp & 0x00ff) == 0x00);
  set_flag(Flags::N, temp & 0x80);

  if (lookup[opcode].address_mode == &CPU::IMP)
    a = temp & 0x00ff;
  else
    write(addr_abs, temp & 0x00ff);
  return 0;
}

auto CPU::BCC() -> uint8_t {
  if (get_flag(Flags::C) == 0) {
    cycles++;
    addr_abs = pc + addr_rel;

    if ((addr_abs & 0xff00) != (pc & 0xff00)) {
      cycles++;
    }
    pc = addr_abs;
  }

  return 0;
}

auto CPU::BCS() -> uint8_t {
  if (get_flag(Flags::C) == 1) {
    cycles++;
    addr_abs = pc + addr_rel;

    if ((addr_abs & 0xff00) != (pc & 0xff00)) {
      cycles++;
    }
    pc = addr_abs;
  }

  return 0;
}

auto CPU::BEQ() -> uint8_t {
  if (get_flag(Flags::Z) == 1) {
    cycles++;
    addr_abs = pc + addr_rel;

    if ((addr_abs & 0xff00) != (pc & 0xff00)) {
      cycles++;
    }
    pc = addr_abs;
  }

  return 0;
}

auto CPU::BIT() -> uint8_t {
  fetch();
  temp = a & fetched;

  set_flag(Flags::Z, (temp & 0x00ff) == 0x00);
  set_flag(Flags::N, fetched & (1 << 7));
  set_flag(Flags::V, fetched & (1 << 6));

  return 0;
}

auto CPU::BMI() -> uint8_t {
  if (get_flag(Flags::N) == 1) {
    cycles++;
    addr_abs = pc + addr_rel;

    if ((addr_abs & 0xff00) != (pc & 0xff00)) {
      cycles++;
    }
    pc = addr_abs;
  }

  return 0;
}

auto CPU::BNE() -> uint8_t {
  if (get_flag(Flags::Z) == 0) {
    cycles++;
    addr_abs = pc + addr_rel;

    if ((addr_abs & 0xff00) != (pc & 0xff00)) {
      cycles++;
    }
    pc = addr_abs;
  }

  return 0;
}

auto CPU::BPL() -> uint8_t {
  if (get_flag(Flags::N) == 0) {
    cycles++;
    addr_abs = pc + addr_rel;

    if ((addr_abs & 0xff00) != (pc & 0xff00)) {
      cycles++;
    }
    pc = addr_abs;
  }

  return 0;
}
// break instruction
auto CPU::BRK() -> uint8_t {
  pc++;

  set_flag(Flags::I, 1);
  write(0x0100 + stkp, (pc >> 8) & 0x00ff);
  stkp--;
  write(0x0100 + stkp, pc & 0x00ff);
  stkp--;

  set_flag(Flags::B, 1);
  write(0x0100 + stkp, status);
  stkp--;
  set_flag(Flags::B, 0);

  pc = (uint16_t)read(0xfffe) | ((uint16_t)read(0xffff) << 8);
  return 0;
}

auto CPU::BVC() -> uint8_t {
  if (get_flag(Flags::V) == 0) {
    cycles++;
    addr_abs = pc + addr_rel;

    if ((addr_abs & 0xff00) != (pc & 0xff00)) {
      cycles++;
    }
    pc = addr_abs;
  }

  return 0;
}

auto CPU::BVS() -> uint8_t {
  if (get_flag(Flags::V) == 1) {
    cycles++;
    addr_abs = pc + addr_rel;

    if ((addr_abs & 0xff00) != (pc & 0xff00)) {
      cycles++;
    }
    pc = addr_abs;
  }

  return 0;
}

auto CPU::CLC() -> uint8_t {
  set_flag(Flags::C, false);
  return 0;
}

auto CPU::CLD() -> uint8_t {
  set_flag(Flags::D, false);
  return 0;
}

auto CPU::CLI() -> uint8_t {
  set_flag(Flags::I, false);
  return 0;
}

auto CPU::CLV() -> uint8_t {
  set_flag(Flags::V, false);
  return 0;
}
// compare accumulator
auto CPU::CMP() -> uint8_t {
  fetch();
  temp = (uint16_t)a - (uint16_t)fetched;

  set_flag(Flags::C, a >= fetched);
  set_flag(Flags::Z, (temp & 0x00ff) == 0x0000);
  set_flag(Flags::N, temp & 0x0080);

  return 1;
}
// compare X register
auto CPU::CPX() -> uint8_t {
  fetch();
  temp = (uint16_t)x - (uint16_t)fetched;

  set_flag(Flags::C, x >= fetched);
  set_flag(Flags::Z, (temp & 0x00ff) == 0x0000);
  set_flag(Flags::N, temp & 0x0080);

  return 1;
}
// compare Y register
auto CPU::CPY() -> uint8_t {
  fetch();
  temp = (uint16_t)y - (uint16_t)fetched;

  set_flag(Flags::C, y >= fetched);
  set_flag(Flags::Z, (temp & 0x00ff) == 0x0000);
  set_flag(Flags::N, temp & 0x0080);

  return 1;
}
// in-place decrement
auto CPU::DEC() -> uint8_t {
  fetch();
  temp = fetched - 1;
  write(addr_abs, temp & 0x00ff);

  set_flag(Flags::Z, (temp & 0x00ff) == 0x0000);
  set_flag(Flags::N, temp & 0x0080);

  return 0;
}
// decrement X register
auto CPU::DEX() -> uint8_t {
  x--;

  set_flag(Flags::Z, x == 0x00);
  set_flag(Flags::N, x & 0x80);

  return 0;
}
// decrement Y register
auto CPU::DEY() -> uint8_t {
  y--;

  set_flag(Flags::Z, y == 0x00);
  set_flag(Flags::N, y & 0x80);

  return 0;
}
// bitwise XOR
auto CPU::EOR() -> uint8_t {
  fetch();
  a ^= fetched;

  set_flag(Flags::Z, a == 0x00);
  set_flag(Flags::N, a & 0x80);

  return 1;
}
// in-place increment
auto CPU::INC() -> uint8_t {
  fetch();
  temp = fetched + 1;
  write(addr_abs, temp & 0x00ff);

  set_flag(Flags::Z, (temp & 0x00ff) == 0x0000);
  set_flag(Flags::N, temp & 0x0080);

  return 0;
}
// increment X register
auto CPU::INX() -> uint8_t {
  x++;

  set_flag(Flags::Z, x == 0x00);
  set_flag(Flags::N, x & 0x80);

  return 0;
}
// increment Y register
auto CPU::INY() -> uint8_t {
  y++;

  set_flag(Flags::Z, y == 0x00);
  set_flag(Flags::N, y & 0x80);

  return 0;
}
// jump to pc location
auto CPU::JMP() -> uint8_t {
  pc = addr_abs;
  return 0;
}
// jump to subroutine
auto CPU::JSR() -> uint8_t {
  pc--;

  write(0x0100 + stkp, (pc >> 8) & 0x00ff);
  stkp--;
  write(0x0100 + stkp, pc & 0x00ff);
  stkp--;

  pc = addr_abs;
  return 0;
}
// load accumulator
auto CPU::LDA() -> uint8_t {
  fetch();
  a = fetched;

  set_flag(Flags::Z, a == 0x00);
  set_flag(Flags::N, a & 0x80);

  return 1;
}
// load X accumulator
auto CPU::LDX() -> uint8_t {
  fetch();
  x = fetched;

  set_flag(Flags::Z, x == 0x00);
  set_flag(Flags::N, x & 0x80);

  return 1;
}
// load Y accumulator
auto CPU::LDY() -> uint8_t {
  fetch();
  y = fetched;

  set_flag(Flags::Z, y == 0x00);
  set_flag(Flags::N, y & 0x80);

  return 1;
}
// logical shift right
auto CPU::LSR() -> uint8_t {
  fetch();

  set_flag(Flags::C, fetched & 0x0001);
  temp = fetched >> 1;
  set_flag(Flags::Z, (temp & 0x00ff) == 0x0000);
  set_flag(Flags::N, temp & 0x0080);

  if (lookup[opcode].address_mode == &CPU::IMP) {
    a = temp & 0x00ff;
  } else {
    write(addr_abs, temp & 0x00ff);
  }

  return 0;
}
// no operation
auto CPU::NOP() -> uint8_t {
  switch (opcode) {
  case 0x1C:
  case 0x3C:
  case 0x5C:
  case 0x7C:
  case 0xDC:
  case 0xFC:
    return 1;
    break;
  }
  return 0;
}
// or with accumulator
auto CPU::ORA() -> uint8_t {
  fetch();
  a |= fetched;

  set_flag(Flags::Z, a == 0x00);
  set_flag(Flags::N, a & 0x80);

  return 1;
}
// push accumulator
auto CPU::PHA() -> uint8_t {
  write(0x0100 + stkp, a);
  stkp--;
  return 0;
}
// push processor status
auto CPU::PHP() -> uint8_t {
  write(0x0100 + stkp, a);
  stkp--;
  return 0;
}
// pull accumulator
auto CPU::PLA() -> uint8_t {
  stkp++;

  a = read(0x0100 + stkp);
  set_flag(Flags::Z, a == 0x00);
  set_flag(Flags::N, a & 0x80);

  return 0;
}
// pull processor status
auto CPU::PLP() -> uint8_t {
  stkp++;
  status = read(0x0100 + stkp);
  set_flag(Flags::U, true);

  return 0;
}
// rotate left
auto CPU::ROL() -> uint8_t {
  fetch();
  temp = (uint16_t)(fetched << 1) | get_flag(Flags::C);

  set_flag(Flags::C, temp & 0xff00);
  set_flag(Flags::Z, (temp & 0x00ff) == 0x0000);
  set_flag(Flags::N, temp & 0x0080);

  if (lookup[opcode].address_mode == &CPU::IMP)
    a = temp & 0x00ff;
  else
    write(addr_abs, temp & 0x00ff);
  return 0;
}
// rotate right
auto CPU::ROR() -> uint8_t {
  fetch();
  temp = (uint16_t)(get_flag(Flags::C) << 7) | (fetched >> 1);
  set_flag(Flags::C, fetched & 0x01);
  set_flag(Flags::Z, (temp & 0x00ff) == 0x00);
  set_flag(Flags::N, temp & 0x0080);
  if (lookup[opcode].address_mode == &CPU::IMP)
    a = temp & 0x00ff;
  else
    write(addr_abs, temp & 0x00ff);
  return 0;
}
// return from interrupt
auto CPU::RTI() -> uint8_t {
  stkp++;
  status = read(0x0100 + stkp);
  status &= ~Flags::B;
  status &= ~Flags::U;

  stkp++;
  pc = (uint16_t)read(0x0100 + stkp);
  stkp++;

  pc |= (uint16_t)read(0x0100 + stkp) << 8;
  return 0;
}
// return from subroutine
auto CPU::RTS() -> uint8_t {
  stkp++;
  pc = (uint16_t)read(0x0100 + stkp);
  stkp++;
  pc |= (uint16_t)read(0x0100 + stkp) << 8;

  pc++;
  return 0;
}
// subtract with carry
auto CPU::SBC() -> uint8_t {
  fetch();

  uint16_t value = ((uint16_t)fetched) ^ 0x00ff;

  temp = (uint16_t)a + value + (uint16_t)get_flag(Flags::C);
  set_flag(Flags::C, temp & 0xFF00);
  set_flag(Flags::Z, ((temp & 0x00ff) == 0));
  set_flag(Flags::V, (temp ^ (uint16_t)a) & (temp ^ value) & 0x0080);
  set_flag(Flags::N, temp & 0x0080);
  a = temp & 0x00ff;
  return 1;
}
// set carry flag
auto CPU::SEC() -> uint8_t {
  set_flag(Flags::C, true);
  return 0;
}
// set decimal flag
auto CPU::SED() -> uint8_t {
  set_flag(Flags::D, true);
  return 0;
}
// set interrup disable flag
auto CPU::SEI() -> uint8_t {
  set_flag(Flags::I, true);
  return 0;
}
// store A register at absolute address
auto CPU::STA() -> uint8_t {
  write(addr_abs, a);
  return 0;
}
// store X register at absolute address
auto CPU::STX() -> uint8_t {
  write(addr_abs, x);
  return 0;
}
// store Y register at absolute address
auto CPU::STY() -> uint8_t {
  write(addr_abs, y);
  return 0;
}
// transfer accumulator to register
auto CPU::TAX() -> uint8_t {
  x = a;

  set_flag(Flags::Z, x == 0x00);
  set_flag(Flags::N, x & 0x80);

  return 0;
}
// transfer accumulator in Y register
auto CPU::TAY() -> uint8_t {
  y = a;

  set_flag(Flags::Z, y == 0x00);
  set_flag(Flags::N, y & 0x80);

  return 0;
}
// transfer stack pointer to X register
auto CPU::TSX() -> uint8_t {
  x = stkp;

  set_flag(Flags::Z, y == 0x00);
  set_flag(Flags::N, y & 0x80);

  return 0;
}
// transfer X register to accumulator
auto CPU::TXA() -> uint8_t {
  a = x;

  set_flag(Flags::Z, a == 0x00);
  set_flag(Flags::N, a & 0x80);

  return 0;
}
// transfer X register to stack pointer
auto CPU::TXS() -> uint8_t {
  stkp = x;
  return 0;
}

auto CPU::TYA() -> uint8_t {
  a = y;

  set_flag(Flags::Z, a == 0x00);
  set_flag(Flags::N, a & 0x80);

  return 0;
}

// handling illegal opcodes
auto CPU::XXX() -> uint8_t { return 0; }

// executes an instruction at one point in time
auto CPU::clock() -> void {
  if (cycles == 0) {
    opcode = read(pc);
    pc += 1;

    cycles = lookup[opcode].cycles;
    uint8_t cycle_1 = (this->*lookup[opcode].address_mode)();
    uint8_t cycle_2 = (this->*lookup[opcode].operate)();

    cycles += (cycle_1 & cycle_2);
  }
  cycles -= 1;
}

auto CPU::reset() -> void {
  a = 0;
  x = 0;
  y = 0;
  stkp = 0xfd;
  status = 0x00 | Flags::U;

  addr_abs = 0xfffc;
  uint16_t low = read(addr_abs + 0);
  uint16_t high = read(addr_abs + 1);

  pc = (high << 8) | low;

  addr_rel = 0x0000;
  addr_abs = 0x0000;
  fetched = 0x00;

  cycles = 8;
}

auto CPU::irq() -> void {
  if (get_flag(Flags::I) == 0) {
    write(0x0100 + stkp, (pc >> 8) & 0x00ff);
    stkp--;
    write(0x0100 + stkp, pc & 0x00ff);
    stkp--;

    set_flag(Flags::B, 0);
    set_flag(Flags::U, 1);
    set_flag(Flags::I, 1);
    write(0x0100 + stkp, status);

    stkp--;

    addr_abs = 0xfffe;
    uint16_t low = read(addr_abs + 0);
    uint16_t high = read(addr_abs + 1);
    pc = (high << 8) | low;

    cycles = 7;
  }
}

auto CPU::nmi() -> void {
  write(0x0100 + stkp, (pc >> 8) & 0x00ff);
  stkp--;
  write(0x0100 + stkp, pc & 0x00ff);
  stkp--;

  set_flag(Flags::B, 0);
  set_flag(Flags::U, 1);
  set_flag(Flags::I, 1);
  write(0x0100 + stkp, status);

  stkp--;

  addr_abs = 0xfffa;
  uint16_t low = read(addr_abs + 0);
  uint16_t high = read(addr_abs + 1);
  pc = (high << 8) | low;

  cycles = 8;
}

auto CPU::fetch() -> uint8_t {
  if (!(lookup[opcode].address_mode == &CPU::IMP)) {
    fetched = read(addr_abs);
  }
  return fetched;
}

auto CPU::is_complete() -> bool { return cycles == 0; }

auto CPU::disassemble(uint16_t start, uint16_t stop)
    -> std::map<uint16_t, std::string> {
  uint8_t high = 0x00;
  uint8_t low = 0x00;
  uint8_t value = 0x00;
  uint8_t opcode = 0x00;

  uint16_t line_addr = 0x0000;

  uint32_t addr = static_cast<uint32_t>(start);
  uint32_t last_addr = static_cast<uint32_t>(stop);

  std::map<uint16_t, std::string> result;

  auto hex = [](uint32_t n, uint8_t d) {
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
      s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
  };

  while (addr <= last_addr) {
    line_addr = addr;

    std::string instruction = "$" + hex(addr, 4) + ": ";
    opcode = m_bus->read_cpu(addr);
    addr++;
    instruction += lookup[opcode].name + " ";

    // switch(lookup[opcode].address_mode) {
    //   case &CPU::IMP: {
    //     instruction += " {IMP}";
    //   } break;

    //   case &CPU::IMM: {
    //     value = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     instruction += "#$" + hex(value, 2) + " {IMM}";
    //   } break;

    //   case &CPU::ZP0: {
    //     low = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     high = 0x00;
    //     instruction += "$" + hex(low, 2) + " {ZP0}";
    //   } break;

    //   case &CPU::ZPX: {
    //     low = m_bus-> read(current_addr);
    //     current_addr++;
    //     high = 0x00;
    //     instruction += "$" + hex(low, 2) + ", X {ZPX}";
    //   } break;

    //   case &CPU::ZPY: {
    //     low = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     high = 0x00;
    //     instruction += "$" + hex(low, 2) + ", Y {ZPY}";
    //   } break;

    //   case &CPU::IZX: {
    //     low = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     high = 0x00;
    //     instruction += "($" + hex(low, 2) + ", X) {IZX}";
    //   } break;

    //   case &CPU::IZY: {
    //     low = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     high = 0x00;
    //     instruction += "($" + hex(low, 2) + ", Y) {IZY}";
    //   } break;

    //   case &CPU::ABS: {
    //     low = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     high = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     instruction += "$" + hex(static_cast<uint16_t>(high << 8) | low, 4) +
    //     " {ABS}";
    //   } break;

    //   case &CPU::ABX: {
    //     low = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     high = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     instruction +=
    //         "$" + hex(static_cast<uint16_t>(high << 8) | low, 4) + ", X
    //         {ABX}";
    //   } break;

    //   case &CPU::ABY: {
    //     low = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     high = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     instruction +=
    //         "$" + hex(static_cast<uint16_t>(high << 8) | low, 4) + ", Y
    //         {ABY}";
    //   } break;

    //   case &CPU::IND: {
    //     low = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     high = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     instruction +=
    //         "($" + hex(static_cast<uint16_t>(high << 8) | low, 4) + ")
    //         {IND}";
    //   } break;

    //   case &CPU::REL: {
    //     value = m_bus->read_cpu(current_addr);
    //     current_addr++;
    //     instruction += "$" + hex(value, 2) + " [$" + hex(current_addr +
    //     value, 4) + "] {REL}";
    //   } break;

    //   default:
    //     break;
    // }

    if (lookup[opcode].address_mode == &CPU::IMP) {
      instruction += " {IMP}";
    } else if (lookup[opcode].address_mode == &CPU::IMM) {
      value = m_bus->read_cpu(addr);
      addr++;
      instruction += "#$" + hex(value, 2) + " {IMM}";
    } else if (lookup[opcode].address_mode == &CPU::ZP0) {
      low = m_bus->read_cpu(addr);
      addr++;
      high = 0x00;
      instruction += "$" + hex(low, 2) + " {ZP0}";
    } else if (lookup[opcode].address_mode == &CPU::ZPX) {
      low = m_bus->read_cpu(addr);
      addr++;
      high = 0x00;
      instruction += "$" + hex(low, 2) + ", X {ZPX}";
    } else if (lookup[opcode].address_mode == &CPU::ZPY) {
      low = m_bus->read_cpu(addr);
      addr++;
      high = 0x00;
      instruction += "$" + hex(low, 2) + ", Y {ZPY}";
    } else if (lookup[opcode].address_mode == &CPU::IZX) {
      low = m_bus->read_cpu(addr);
      addr++;
      high = 0x00;
      instruction += "($" + hex(low, 2) + ", X) {IZX}";
    } else if (lookup[opcode].address_mode == &CPU::IZY) {
      low = m_bus->read_cpu(addr);
      addr++;
      high = 0x00;
      instruction += "($" + hex(low, 2) + "), Y {IZY}";
    } else if (lookup[opcode].address_mode == &CPU::ABS) {
      low = m_bus->read_cpu(addr);
      addr++;
      high = m_bus->read_cpu(addr);
      addr++;
      instruction += "$" + hex((uint16_t)(high << 8) | low, 4) + " {ABS}";
    } else if (lookup[opcode].address_mode == &CPU::ABX) {
      low = m_bus->read_cpu(addr);
      addr++;
      high = m_bus->read_cpu(addr);
      addr++;
      instruction += "$" + hex((uint16_t)(high << 8) | low, 4) + ", X {ABX}";
    } else if (lookup[opcode].address_mode == &CPU::ABY) {
      low = m_bus->read_cpu(addr);
      addr++;
      high = m_bus->read_cpu(addr);
      addr++;
      instruction += "$" + hex((uint16_t)(high << 8) | low, 4) + ", Y {ABY}";
    } else if (lookup[opcode].address_mode == &CPU::IND) {
      low = m_bus->read_cpu(addr);
      addr++;
      high = m_bus->read_cpu(addr);
      addr++;
      instruction += "($" + hex((uint16_t)(high << 8) | low, 4) + ") {IND}";
    } else if (lookup[opcode].address_mode == &CPU::REL) {
      value = m_bus->read_cpu(addr);
      addr++;
      instruction += "$" + hex(value, 2) + " [$" +
                     hex(addr + (int8_t)value, 4) + "] {REL}";
    }

    result[line_addr] = instruction;
  }

  return result;
}

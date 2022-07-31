#include "CPU.h"
#include <cstdint>

CPU::CPU() = default;
CPU::~CPU() = default;

auto CPU::connect(const Bus *bus) -> void{
    m_bus = bus;
}

auto CPU::read(uint16_t addr) -> uint8_t{
    m_bus->read(addr);
}

auto CPU::write(uint16_t addr, uint8_t data) -> void {
    m_bus->write(addr, data);
}

// addressing modes
auto CPU::IMP() -> uint8_t;
auto CPU::ZP0() -> uint8_t;
auto CPU::ZPY() -> uint8_t;
auto CPU::ABS() -> uint8_t;
auto CPU::ABY() -> uint8_t;
auto CPU::IZX() -> uint8_t;
auto CPU::IMM() -> uint8_t;
auto CPU::ZPX() -> uint8_t;
auto CPU::REL() -> uint8_t;
auto CPU::ABX() -> uint8_t;
auto CPU::IMD() -> uint8_t;
auto CPU::IZY() -> uint8_t;

// legal opcodes defined in the NES
auto CPU::ADC() -> uint8_t;
auto CPU::AND() -> uint8_t;
auto CPU::ASL() -> uint8_t;
auto CPU::BCC() -> uint8_t;
auto CPU::BCS() -> uint8_t;
auto CPU::BEQ() -> uint8_t;
auto CPU::BIT() -> uint8_t;
auto CPU::BMI() -> uint8_t;
auto CPU::BNE() -> uint8_t;
auto CPU::BPL() -> uint8_t;
auto CPU::BRK() -> uint8_t;
auto CPU::BVC() -> uint8_t;
auto CPU::BVS() -> uint8_t;
auto CPU::CLC() -> uint8_t;
auto CPU::CLD() -> uint8_t;
auto CPU::CLI() -> uint8_t;
auto CPU::CLV() -> uint8_t;
auto CPU::CMP() -> uint8_t;
auto CPU::CPX() -> uint8_t;
auto CPU::CPY() -> uint8_t;
auto CPU::DEC() -> uint8_t;
auto CPU::DEX() -> uint8_t;
auto CPU::DEY() -> uint8_t;
auto CPU::EOR() -> uint8_t;
auto CPU::INC() -> uint8_t;
auto CPU::INX() -> uint8_t;
auto CPU::INY() -> uint8_t;
auto CPU::JMP() -> uint8_t;
auto CPU::JSR() -> uint8_t;
auto CPU::LDA() -> uint8_t;
auto CPU::LDX() -> uint8_t;
auto CPU::LDY() -> uint8_t;
auto CPU::LSR() -> uint8_t;
auto CPU::NOP() -> uint8_t;
auto CPU::ORA() -> uint8_t;
auto CPU::PHA() -> uint8_t;
auto CPU::PHP() -> uint8_t;
auto CPU::PLA() -> uint8_t;
auto CPU::PLP() -> uint8_t;
auto CPU::ROL() -> uint8_t;
auto CPU::ROR() -> uint8_t;
auto CPU::RTI() -> uint8_t;
auto CPU::RTS() -> uint8_t;
auto CPU::SBC() -> uint8_t;
auto CPU::SEC() -> uint8_t;
auto CPU::SED() -> uint8_t;
auto CPU::SEI() -> uint8_t;
auto CPU::STA() -> uint8_t;
auto CPU::STX() -> uint8_t;
auto CPU::STY() -> uint8_t;
auto CPU::TAX() -> uint8_t;
auto CPU::TAY() -> uint8_t;
auto CPU::TSX() -> uint8_t;
auto CPU::TXA() -> uint8_t;
auto CPU::TXS() -> uint8_t;
auto CPU::TYA() -> uint8_t;

// handling illegal opcodes
auto CPU::XXX() -> uint8_t;

auto CPU::clock() -> void {
    if(cycles == 0) { 
        opcode = read(pc);
        pc += 1;

        cycles = lookup[opcode].cycles;
        lookup[opcode].address_mode()
    }
    cycles -= 1; 
}

auto CPU::reset() -> void; 
auto CPU::irq() -> void;   
auto CPU::nmi() -> void;   

auto CPU::fetch() -> uint8_t;
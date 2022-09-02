#include "../include/Mappers.hpp"

#include <cstdint>

Mapper::Mapper(uint8_t prg_banks, uint8_t char_bank)
    : m_prg_banks(prg_banks), m_char_banks(char_bank) {}

Mapper_000::Mapper_000(uint8_t prg_banks, uint8_t char_bank)
    : Mapper(prg_banks, char_bank) {}

auto Mapper_000::read_cpu(uint16_t address, uint32_t &mapped_addr) -> bool {
  if (address >= 0x8000 && address <= 0xffff) {
    mapped_addr = address & (m_prg_banks > 1 ? 0x7fff : 0x3fff);
    return true;
  }
  return false;
}

auto Mapper_000::write_cpu(uint16_t address, uint32_t &mapped_addr) -> bool {
  if (address >= 0x8000 && address <= 0xffff) {
    mapped_addr = address & (m_prg_banks > 1 ? 0x7fff : 0x3fff);
    return true;
  }
  return false;
}

auto Mapper_000::read_ppu(uint16_t address, uint32_t &mapped_addr) -> bool {
  if (address >= 0x0000 && address <= 0x1fff) {
    mapped_addr = address;
    return true;
  }
  return false;
}

auto Mapper_000::write_ppu(uint16_t address, uint32_t &mapped_addr) -> bool {
  if (address >= 0x0000 && address <= 0x1fff) {
    if (m_char_banks == 0) {
      // treated as RAM
      mapped_addr = address;
      return true;
    }
  }
  return false;
}
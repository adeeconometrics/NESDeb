#ifndef __MAPPERS_H__
#define __MAPPERS_H__

#include <cstdint>

class Mapper {
public:
  Mapper() = delete;
  Mapper(const Mapper &) = delete;
  Mapper(Mapper &&) = delete;

  Mapper(uint8_t prg_banks, uint8_t char_bank);
  virtual ~Mapper() = default;

  virtual auto read_cpu(uint16_t address, uint32_t &mapped_addr) -> bool = 0;
  virtual auto write_cpu(uint16_t address, uint32_t &mapped_addr) -> bool = 0;

  virtual auto read_ppu(uint16_t address, uint32_t &mapped_addr) -> bool = 0;
  virtual auto write_ppu(uint16_t address, uint32_t &mapped_addr) -> bool = 0;

protected:
  uint8_t m_prg_banks{};
  uint8_t m_char_banks{};
};

#endif // __MAPPERS_H__
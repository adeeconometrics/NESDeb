#ifndef __MAPPERS_H__
#define __MAPPERS_H__

#include <cstdint>

// the purpose of a mapper is to translate the address

class Mapper {
public:
  // Mapper() = delete;
  Mapper(const Mapper &) = delete;
  Mapper(Mapper &&) = delete;

  Mapper(uint8_t prg_banks, uint8_t char_bank);
  virtual ~Mapper() = default;

  auto operator=(const Mapper&) -> Mapper& = delete;
  auto operator=(Mapper&&) -> Mapper& = delete;

  virtual auto read_cpu(uint16_t address, uint32_t &mapped_addr) -> bool = 0;
  virtual auto write_cpu(uint16_t address, uint32_t &mapped_addr) -> bool = 0;

  virtual auto read_ppu(uint16_t address, uint32_t &mapped_addr) -> bool = 0;
  virtual auto write_ppu(uint16_t address, uint32_t &mapped_addr) -> bool = 0;

protected:
  uint8_t m_prg_banks{};
  uint8_t m_char_banks{};
};

class Mapper_000: public Mapper {

public:
  Mapper_000(uint8_t prg_banks, uint8_t char_bank);

  auto read_cpu(uint16_t address, uint32_t &mapped_addr) -> bool override;
  auto write_cpu(uint16_t address, uint32_t &mapped_addr) -> bool override;
  auto read_ppu(uint16_t address, uint32_t &mapped_addr) -> bool override;
  auto write_ppu(uint16_t address, uint32_t &mapped_addr) -> bool override;
};

#endif // __MAPPERS_H__
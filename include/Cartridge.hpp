#ifndef __CARTRIDGE_H__
#define __CARTRIDGE_H__

#include "Mappers.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Cartridge {
private:
  struct Header {
    uint8_t prg_rom_chunks;
    uint8_t chr_rom_chunks;
    uint8_t mapper_1;
    uint8_t mapper_2;
    uint8_t prg_ram_size;
    uint8_t tv_system_1;
    uint8_t tv_system_2;

    std::array<char, 5> unused;
  } header;

public:
  Cartridge(const std::string &fname);
  ~Cartridge() = default;

  auto read_cpu(uint16_t address, uint8_t &data) -> bool;
  auto write_cpu(uint16_t address, uint8_t data) -> bool;

  auto read_ppu(uint16_t address, uint8_t &data) -> bool;
  auto write_ppu(uint16_t address, uint8_t data) -> bool;

  auto is_valid_image() -> bool;

private:
  std::shared_ptr<Mapper> m_mapper;
  std::vector<uint8_t> m_prg_memory;
  std::vector<uint8_t> m_chr_memory;

  uint8_t m_mapper_id{};
  uint8_t m_prg_banks{};
  uint8_t m_chr_banks{};

  bool m_is_valid_image = false;
};
#endif // __CARTRIDGE_H__
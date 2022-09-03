#ifndef __BUS_H__
#define __BUS_H__

#include "CPU.hpp"
#include "Cartridge.hpp"
#include "PPU.hpp"

#include <array>
#include <cstdint>
#include <memory>

class Cartridge;
class CPU;
class PPU;

class Bus final {
public:
  std::unique_ptr<CPU> m_cpu;
  std::unique_ptr<PPU> m_ppu;
  std::shared_ptr<Cartridge> m_cartridge;
  std::array<uint8_t, 2048> m_cpu_ram; // 2kB

public:
  Bus();
  ~Bus();

  auto write_cpu(uint16_t address, uint8_t data) -> void;
  auto read_cpu(uint16_t address) -> uint8_t;

  auto insert_cartridge(const std::shared_ptr<Cartridge> &cartridge) -> void;
  auto reset() -> void;
  auto clock() -> void;

private:
  uint32_t m_system_clock_counter{};
};

#endif // __BUS_H__
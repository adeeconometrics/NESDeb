#ifndef __PPU_H__
#define __PPU_H__

#include "Cartridge.h"

#include <cstdint>
#include <memory>

class PPU {
public:
  PPU() = default; // subject to implementation
  ~PPU() = default;

  auto read_cpu(uint16_t address, bool is_read_only = false) -> uint8_t;
  auto write_cpu(uint16_t address, uint8_t data) -> void;

  auto read_ppu(uint16_t address, bool is_read_only = false) -> uint8_t;
  auto write_ppu(uint16_t address, uint8_t data) -> void;

  auto connect(const std::shared_ptr<Cartridge> &cartridge) -> void;
  auto clock() -> void;

private:
  bool m_is_frame_complete = false;

  const int Control = 0x0000;
  const int Mask = 0x0001;
  const int Status = 0x0002;
  const int OAM_Address = 0x0003;
  const int OAM_Data = 0x0004;
  const int Scroll = 0x0005;
  const int PPU_Address = 0x0006;
  const int PPU_Data = 0x0007;

  int16_t m_scan_line = 0;
  int16_t m_cycle = 0;

  std::shared_ptr<Cartridge> m_cartridge;
};

#endif // __PPU_H__
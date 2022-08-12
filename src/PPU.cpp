#include "PPU.h"
#include "Cartridge.h"

#include <cstdint>
#include <memory>

auto PPU::read_cpu(uint16_t address, bool is_read_only = false) -> uint8_t {

  uint8_t data = 0x00;

  switch (address) {
  case Control:
    break;
  case Mask:
    break;
  case Status:
    break;
  case OAM_Address:
    break;
  case OAM_Data:
    break;
  case Scroll:
    break;
  case PPU_Address:
    break;
  case PPU_Data:
    break;
  };
  return data;
}

auto PPU::write_cpu(uint16_t address, uint8_t data) -> void {
  switch (address) {
  case Control:
    break;
  case Mask:
    break;
  case Status:
    break;
  case OAM_Address:
    break;
  case OAM_Data:
    break;
  case Scroll:
    break;
  case PPU_Address:
    break;
  case PPU_Data:
    break;
  };
}

auto PPU::read_ppu(uint16_t address, bool is_read_only = false) -> uint8_t {
    uint8_t data = 0x00l
    address &= 0x3fff;

  return data;
}

auto PPU::write_ppu(uint16_t address, uint8_t data) -> void {
    address &= 0x3fff;
}

auto PPU::connect(const std::shared_ptr<Cartridge> &cartridge) -> void {
    this->m_cartridge = cartridge;
}

auto PPU::clock() -> void {
    m_cycle ++; 
    if (m_cycle >= 341) {
        m_cycle = 0;
        m_scan_line += 1;

        if(m_scan_line >= 261) {
            m_scan_line -= 1;
            m_is_frame_complete = true;
        }
    }
}
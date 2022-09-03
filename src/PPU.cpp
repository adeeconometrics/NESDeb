#include "../include/PPU.hpp"
#include "../include/Cartridge.hpp"
#include "../include/olcPixelGameEngine.hpp"

#include <cstdint>
#include <memory>

auto PPU::read_cpu(uint16_t address, bool is_read_only = false) -> uint8_t {

  uint8_t data = 0x00;

  switch (static_cast<PPUConstants>(address)) {
  case PPUConstants::Control:
    break;
  case PPUConstants::Mask:
    break;
  case PPUConstants::PStatus:
    break;
  case PPUConstants::OAM_Address:
    break;
  case PPUConstants::OAM_Data:
    break;
  case PPUConstants::Scroll:
    break;
  case PPUConstants::PPU_Address:
    break;
  case PPUConstants::PPU_Data:
    break;
  };
  return data;
}

auto PPU::write_cpu(uint16_t address, uint8_t data) -> void {
  switch (static_cast<PPUConstants>(address)) {
  case PPUConstants::Control:
    break;
  case PPUConstants::Mask:
    break;
  case PPUConstants::PStatus:
    break;
  case PPUConstants::OAM_Address:
    break;
  case PPUConstants::OAM_Data:
    break;
  case PPUConstants::Scroll:
    break;
  case PPUConstants::PPU_Address:
    break;
  case PPUConstants::PPU_Data:
    break;
  };
}

auto PPU::read_ppu(uint16_t address, bool is_read_only = false) -> uint8_t {
  uint8_t data = 0x00l;
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
  m_cycle++;
  if (m_cycle >= 341) {
    m_cycle = 0;
    m_scan_line += 1;

    if (m_scan_line >= 261) {
      m_scan_line -= 1;
      m_is_frame_complete = true;
    }
  }
}

auto PPU::get_screen() -> olc::Sprite & { return m_screen; }

auto PPU::get_table_name(uint8_t i) -> olc::Sprite & {
  return m_spr_table_name.at(i);
}

auto PPU::get_table_pattern(uint8_t i) -> olc::Sprite & {
  return m_spr_table_pattern.at(i);
}
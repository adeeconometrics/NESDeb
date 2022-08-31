#ifndef __PPU_H__
#define __PPU_H__

#include "olcPixelGameEngine.h"
#include "Cartridge.h"

#include <cstdint>
#include <memory>
#include <array>

class PPU {
public:
  PPU() = default; // subject to implementation
  ~PPU() = default;

  auto read_cpu(uint16_t address, bool is_read_only) -> uint8_t;
  auto write_cpu(uint16_t address, uint8_t data) -> void;

  auto read_ppu(uint16_t address, bool is_read_only) -> uint8_t;
  auto write_ppu(uint16_t address, uint8_t data) -> void;

  auto connect(const std::shared_ptr<Cartridge> &cartridge) -> void;
  auto clock() -> void;

public:
  // debugging tools
  auto get_screen() -> olc::Sprite&;
  auto get_table_name(uint8_t i) -> olc::Sprite&;
  auto get_table_pattern(uint8_t i) -> olc::Sprite&;

private: 
  std::array<std::array<uint8_t, 4096>, 2> table_pattern {};
  std::array<std::array<uint8_t, 1024>, 2> table_name{};
  std::array<uint8_t, 32> table_pallette{};

  const std::array<olc::Pixel, 64> m_pal_screen {
                                olc::Pixel{84, 84, 84},
                                olc::Pixel{0, 30, 116},
                                olc::Pixel{8, 16, 144},
                                olc::Pixel{48, 0, 136},
                                olc::Pixel{68, 0, 100},
                                olc::Pixel{92, 0, 48},
                                olc::Pixel{84, 4, 0},
                                olc::Pixel{60, 24, 0},
                                olc::Pixel{32, 42, 0},
                                olc::Pixel{8, 58, 0},
                                olc::Pixel{0, 64, 0},
                                olc::Pixel{0, 60, 0},
                                olc::Pixel{0, 50, 60},
                                olc::Pixel{0, 0, 0},
                                olc::Pixel{0, 0, 0},
                                olc::Pixel{0, 0, 0},

                                olc::Pixel{152, 150, 152},
                                olc::Pixel{8, 76, 196},
                                olc::Pixel{48, 50, 236},
                                olc::Pixel{92, 30, 228},
                                olc::Pixel{136, 20, 176},
                                olc::Pixel{160, 20, 100},
                                olc::Pixel{152, 34, 32},
                                olc::Pixel{120, 60, 0},
                                olc::Pixel{84, 90, 0},
                                olc::Pixel{40, 114, 0},
                                olc::Pixel{8, 124, 0},
                                olc::Pixel{0, 118, 40},
                                olc::Pixel{0, 102, 120},
                                olc::Pixel{0, 0, 0},
                                olc::Pixel{0, 0, 0},
                                olc::Pixel{0, 0, 0},

                                olc::Pixel{236, 238, 236},
                                olc::Pixel{76, 154, 236},
                                olc::Pixel{120, 124, 236},
                                olc::Pixel{176, 98, 236},
                                olc::Pixel{228, 84, 236},
                                olc::Pixel{236, 88, 180},
                                olc::Pixel{236, 106, 100},
                                olc::Pixel{212, 136, 32},
                                olc::Pixel{160, 170, 0},
                                olc::Pixel{116, 196, 0},
                                olc::Pixel{76, 208, 32},
                                olc::Pixel{56, 204, 108},
                                olc::Pixel{56, 180, 204},
                                olc::Pixel{60, 60, 60},
                                olc::Pixel{0, 0, 0},
                                olc::Pixel{0, 0, 0},

                                olc::Pixel{236, 238, 236},
                                olc::Pixel{168, 204, 236},
                                olc::Pixel{188, 188, 236},
                                olc::Pixel{212, 178, 236},
                                olc::Pixel{236, 174, 236},
                                olc::Pixel{236, 174, 212},
                                olc::Pixel{236, 180, 176},
                                olc::Pixel{228, 196, 144},
                                olc::Pixel{204, 210, 120},
                                olc::Pixel{180, 222, 120},
                                olc::Pixel{168, 226, 144},
                                olc::Pixel{152, 226, 180},
                                olc::Pixel{160, 214, 228},
                                olc::Pixel{160, 162, 160},
                                olc::Pixel{0, 0, 0},
                                olc::Pixel{0, 0, 0}
  };
  
  
  std::array<olc::Sprite, 2> m_spr_table_name {
    olc::Sprite{256,240}, olc::Sprite{256,240}
  };
  std::array<olc::Sprite, 2> m_spr_table_pattern {
    olc::Sprite{128,128}, olc::Sprite{128,128}
  };
  
  olc::Sprite m_screen{256,240};

private:
  enum class PPUConstants: int {
    Control = 0x0000,
    Mask = 0x0001,
    Status = 0x0002,
    OAM_Address = 0x0003,
    OAM_Data = 0x0004,
    Scroll = 0x0005,
    PPU_Address = 0x0006,
    PPU_Data = 0x0007
  };

  int16_t m_scan_line = 0;
  int16_t m_cycle = 0;

  bool m_is_frame_complete = false;

  std::shared_ptr<Cartridge> m_cartridge;
};

#endif // __PPU_H__
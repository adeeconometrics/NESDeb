#include "../include/Bus.hpp"
#include "../include/CPU.hpp"

#include <iostream>
#include <sstream>

#define OLC_PGE_APPLICATION
#include "../include/olcPixelGameEngine.hpp"

class Demo_olc2C02 : public olc::PixelGameEngine {
public:
  Demo_olc2C02() { sAppName = "olc2C02 Demonstration"; }

private:
  // The NES
  Bus m_nes;
  std::shared_ptr<Cartridge> m_cart;
  bool is_running = false;
  float residual_time = 0.0f;

private:
  // Support Utilities
  std::map<uint16_t, std::string> map_asm;

  std::string hex(uint32_t n, uint8_t d) {
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
      s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
  };

  void DrawRam(int x, int y, uint16_t addr, int rows, int cols) {
    const int nRamX = x;
    int nRamY = y;
    for (int row = 0; row < rows; row++) {
      std::string str_offset = "$" + hex(addr, 4) + ":";
      for (int col = 0; col < cols; col++) {
        str_offset += " " + hex(m_nes.read_cpu(addr), 2);
        addr += 1;
      }
      DrawString(nRamX, nRamY, str_offset);
      nRamY += 10;
    }
  }

  void DrawCpu(int x, int y) {
    std::string status = "STATUS: ";
    DrawString(x, y, "STATUS:", olc::WHITE);
    DrawString(x + 64, y, "N",
               m_nes.m_cpu->status & CPU::Flags::N ? olc::GREEN : olc::RED);
    DrawString(x + 80, y, "V",
               m_nes.m_cpu->status & CPU::Flags::V ? olc::GREEN : olc::RED);
    DrawString(x + 96, y, "-",
               m_nes.m_cpu->status & CPU::Flags::U ? olc::GREEN : olc::RED);
    DrawString(x + 112, y, "B",
               m_nes.m_cpu->status & CPU::Flags::B ? olc::GREEN : olc::RED);
    DrawString(x + 128, y, "D",
               m_nes.m_cpu->status & CPU::Flags::D ? olc::GREEN : olc::RED);
    DrawString(x + 144, y, "I",
               m_nes.m_cpu->status & CPU::Flags::I ? olc::GREEN : olc::RED);
    DrawString(x + 160, y, "Z",
               m_nes.m_cpu->status & CPU::Flags::Z ? olc::GREEN : olc::RED);
    DrawString(x + 178, y, "C",
               m_nes.m_cpu->status & CPU::Flags::C ? olc::GREEN : olc::RED);
    DrawString(x, y + 10, "PC: $" + hex(m_nes.m_cpu->pc, 4));
    DrawString(x, y + 20,
               "A: $" + hex(m_nes.m_cpu->a, 2) + "  [" +
                   std::to_string(m_nes.m_cpu->a) + "]");
    DrawString(x, y + 30,
               "X: $" + hex(m_nes.m_cpu->x, 2) + "  [" +
                   std::to_string(m_nes.m_cpu->x) + "]");
    DrawString(x, y + 40,
               "Y: $" + hex(m_nes.m_cpu->y, 2) + "  [" +
                   std::to_string(m_nes.m_cpu->y) + "]");
    DrawString(x, y + 50, "Stack P: $" + hex(m_nes.m_cpu->stkp, 4));
  }

  void DrawCode(int x, int y, int nLines) {
    auto it_a = map_asm.find(m_nes.m_cpu->pc);
    int nLineY = (nLines >> 1) * 10 + y;
    if (it_a != map_asm.end()) {
      DrawString(x, nLineY, (*it_a).second, olc::CYAN);
      while (nLineY < (nLines * 10) + y) {
        nLineY += 10;
        if (++it_a != map_asm.end()) {
          DrawString(x, nLineY, (*it_a).second);
        }
      }
    }

    it_a = map_asm.find(m_nes.m_cpu->pc);
    nLineY = (nLines >> 1) * 10 + y;
    if (it_a != map_asm.end()) {
      while (nLineY > y) {
        nLineY -= 10;
        if (--it_a != map_asm.end()) {
          DrawString(x, nLineY, (*it_a).second);
        }
      }
    }
  }

  bool OnUserCreate() {
    // Load the cartridge
    m_cart = std::make_shared<Cartridge>("nestest.m_nes");
    if (!m_cart->is_valid_image())
      return false;

    // Insert into NES
    m_nes.insert_cartridge(m_cart);

    // Extract dissassembly
    map_asm = m_nes.m_cpu->disassemble(0x0000, 0xFFFF);

    // Reset NES
    m_nes.reset();
    return true;
  }

  bool OnUserUpdate(float elapsed_time) {
    Clear(olc::DARK_BLUE);

    if (is_running) {
      if (residual_time > 0.0f)
        residual_time -= elapsed_time;
      else {
        residual_time += (1.0f / 60.0f) - elapsed_time;
        do {
          m_nes.clock();
        } while (!m_nes.m_ppu->m_is_frame_complete);
        m_nes.m_ppu->m_is_frame_complete = false;
      }
    } else {
      // Emulate code step-by-step
      if (GetKey(olc::Key::C).bPressed) {
        // Clock enough times to execute a whole CPU instruction
        do {
          m_nes.clock();
        } while (!m_nes.m_cpu->is_complete());
        // CPU clock runs slower than system clock, so it may be
        // complete for additional system clock cycles. Drain
        // those out
        do {
          m_nes.clock();
        } while (m_nes.m_cpu->is_complete());
      }

      // Emulate one whole frame
      if (GetKey(olc::Key::F).bPressed) {
        // Clock enough times to draw a single frame
        do {
          m_nes.clock();
        } while (!m_nes.m_ppu->m_is_frame_complete);
        // Use residual clock cycles to complete current instruction
        do {
          m_nes.clock();
        } while (!m_nes.m_cpu->is_complete());
        // Reset frame completion flag
        m_nes.m_ppu->m_is_frame_complete = false;
      }
    }

    if (GetKey(olc::Key::SPACE).bPressed)
      is_running = !is_running;
    if (GetKey(olc::Key::R).bPressed)
      m_nes.reset();

    DrawCpu(516, 2);
    DrawCode(516, 72, 26);

    DrawSprite(0, 0, &m_nes.m_ppu->get_screen(), 2);
    return true;
  }
};

int main() {
  Demo_olc2C02 demo;
  demo.Construct(780, 480, 2, 2);
  demo.Start();
  return 0;
}
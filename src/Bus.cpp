#include "Bus.h"
#include "CPU.h"

Bus::Bus() {
    // initialize RAM to 0
    for (auto &i: m_ram){ i = 0x00;}
    m_cpu.connect(this);
}

Bus::~Bus() = default; 

auto Bus::write(uint16_t address, uint8_t data) -> void {
  if (address >= 0x0000 && address <= 0xffff) {
    m_ram[address] = data;
  }
}

auto Bus::read(uint16_t address, bool is_read_only) -> uint8_t {
  if (address >= 0x0000 && address <= 0xffff) {
    return m_ram[data];
  }
  return 0x00;
}
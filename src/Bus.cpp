#include "Bus.h"
#include "Cartridge.h"

#include <cstdint>
#include <memory>

Bus::Bus() : m_cpu(std::make_unique<CPU>()), m_ppu(std::make_unique<PPU>()) {
  // initialize RAM to 0
  (*m_cpu).connect(this);
  for (auto &i : m_cpu_ram) {
    i = 0x00;
  }
}

Bus::~Bus() = default;

auto Bus::write_cpu(uint16_t address, uint8_t data) -> void {
  if (m_cartridge -> write_cpu(address, data)){
  // configure for cartridge address range       
  }

  else if (address >= 0x0000 && address <= 0xffff) {
    m_cpu_ram[address & 0x07ff] = data;
  }

  else if (address >= 0x2000 && address <= 0x3fff) {
    (*m_ppu).write_cpu(address & 0x0007, data);
  }
}

auto Bus::read_cpu(uint16_t address) -> uint8_t {
  uint8_t data = 0x00;
  if (m_cartridge -> write_cpu(address, data)){
  // configure for cartridge address range       
  }

  else if (address >= 0x0000 && address <= 0x1fff) {
    return m_cpu_ram[address & 0x7ff]; // adjusted for RAM mirorring
  }

  else if (address >= 0x2000 && address <= 0x3fff) {
    (*m_ppu).read_cpu(address & 0x0007, data);
  }
  return data;
}

auto Bus::insert_cartridge(const std::shared_ptr<Cartridge> &cartridge)
    -> void {
  this->m_cartridge = cartridge;
  (*m_ppu).connect(cartridge);
}

auto Bus::reset() -> void {
  (*m_cpu).reset();
  m_system_clock_counter = 0;
}

auto Bus::clock() -> void {
  (*m_ppu).clock();

  if (m_system_clock_counter % 3 == 0) {
    (*m_cpu).clock();
  }

  m_system_clock_counter += 1;
}

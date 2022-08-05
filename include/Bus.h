#ifndef __BUS_H__
#define __BUS_H__

#include <cstdint>
#include <array>
#include <memory>

#include "CPU.h"

class CPU;

class Bus final {
private:

public: 
    std::unique_ptr<CPU> m_cpu;
    std::array<uint8_t, 64*1024> m_ram; // 64kB

public:
  Bus();
  ~Bus();

  auto write(uint16_t address, uint8_t data) -> void;
  auto read(uint16_t address) -> uint8_t;
};



#endif // __BUS_H__
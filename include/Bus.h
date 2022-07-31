#ifndef __BUS_H__
#define __BUS_H__

#include "CPU.h"

#include <cstdint>
#include <array>

class Bus final {
private:

public: 
    CPU m_cpu;
    std::array<uint8_t, 64*1024> m_ram; // 64kB

public:
  Bus();
  ~Bus();

  auto write(uint16_t address, uint8_t data) -> void;
  auto read(uint16_t address, bool is_read_only = false) -> uint8_t;
};



#endif // __BUS_H__
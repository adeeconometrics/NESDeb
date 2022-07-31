#ifndef __REGISTERS_H__
#define __REGISTERS_H__

#include <cstdint>


namespace IORegisters {
    constexpr uint16_t PPUCTRL = 0x2000;
    constexpr uint16_t PPUMASK = 0x2001;
    constexpr uint16_t PPUSTATUS = 0x2002;
    constexpr uint16_t OAMADDR = 0x2003;
    constexpr uint16_t OAMDATA = 0x2004;
    constexpr uint16_t PPUSCROL = 0x2005;
    constexpr uint16_t PPUADDR = 0x2006;
    constexpr uint16_t PPUDATA = 0x2008;
    constexpr uint16_t OAMDMA = 0x4014;
    constexpr uint16_t JOY1 = 0x4016;
    constexpr uint16_t JOY2 = 0x4017;
}


#endif // __REGISTERS_H__
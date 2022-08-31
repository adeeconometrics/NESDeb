#include "Cartridge.h"
#include "Mappers.h"

#include <cstdint>
#include <fstream>

Cartridge::Cartridge(const std::string &fname) {
    std::ifstream stream;
    stream.open(fname, std::ifstream::binary);

    if(stream.is_open()){
        // read the file header
        stream.read((char*)&header, sizeof(Header));

        if(header.mapper_1 & 0x04) {
            stream.seekg(512, std::ios_base::cur);
        }

        // set mapper id
        m_mapper_id = ((header.mapper_2 >> 4) << 4) | (header.mapper_1 >> 4);

        uint8_t ftype = 1; 

        if(ftype == 0) {

        } 
        if(ftype == 1) {
            m_prg_banks = header.prg_rom_chunks;
            m_prg_memory.resize(m_prg_banks * 16384);
            stream.read((char*)m_prg_memory.data(), m_prg_memory.size());

            m_chr_banks = header.chr_rom_chunks;
            m_chr_memory.resize(m_chr_banks * 8192);
            stream.read((char*)m_chr_memory.data(), m_chr_memory.size());
        }

        if(ftype == 2) {

        }

        // load mapper 
        switch(m_mapper_id){
            case 0: 
                m_mapper = std::make_shared<Mapper_000>(m_prg_banks, m_chr_banks);
                break;
        }

        m_is_valid_image = true; 
        stream.close();
    }
}

auto Cartridge::read_cpu(uint16_t address, uint8_t &data)
    -> bool {
        
        uint32_t mapped_addr = 0;
        if(m_mapper -> read_cpu(address, mapped_addr)){
            data = m_prg_memory[mapped_addr];
            return true;
        }

        return false;
}

auto Cartridge::write_cpu(uint16_t address, uint8_t data) -> bool {
  uint32_t mapped_addr = 0;
  if (m_mapper->write_cpu(address, mapped_addr)) {
    m_prg_memory[mapped_addr] = data;
    return true;
  }

  return false;
}

auto Cartridge::read_ppu(uint16_t address, uint8_t &data)
    -> bool {

  uint32_t mapped_addr = 0;
  if (m_mapper->read_ppu(address, mapped_addr)) {
    data = m_chr_memory[mapped_addr];
    return true;
  }

  return false;
}

auto Cartridge::write_ppu(uint16_t address, uint8_t data) -> bool {
  uint32_t mapped_addr = 0;
  if (m_mapper->write_ppu(address, mapped_addr)) {
    m_chr_memory[mapped_addr] = data;
    return true;
  }

  return false;
}

auto Cartridge::is_valid_image() -> bool { 
    return m_is_valid_image;
}

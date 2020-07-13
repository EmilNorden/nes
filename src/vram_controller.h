#ifndef NES_VRAM_CONTROLLER_H
#define NES_VRAM_CONTROLLER_H

#include <cstdint>
#include <cstring>
#include <array>
#include <algorithm>
#include "ppu_registers.h"

class vram_controller {
 public:
  explicit vram_controller(ppu_registers& regs) : m_regs(regs) {
    std::fill(std::begin(m_memory), std::end(m_memory), 0);
  }
 /*constexpr void ppuctrl_write(std::uint8_t value) noexcept {
   auto nametable_index = value & 0x03U;
   m_nametable_base_address = static_cast<std::uint16_t>(0x2000U + (nametable_index * 0x400U));
   m_vram_address_increment =  (value & 0x04U) ? 32 : 1;
   m_sprite_pattern_table_address = (value & 0x08U) ? 0x1000 : 0x0000;
   m_bg_pattern_table_address = (value & 0x10) ? 0x1000 : 0x0000;
   // sprite size is on flag 0x20, 0 is 8x8, 1 is 8x16. Not handling this right now
   // ppu master/slave is on flag 0x40, (0: read backdrop from EXT pins; 1: output color on EXT pins). Not handling this either
   m_generate_vblank_nmi = (value & 0x80);
 }*/

 [[nodiscard]] constexpr std::uint8_t read_nametable(unsigned int x, unsigned int y) noexcept {
   return m_memory[m_regs.ppuctrl_nametable_base_address() + x + (y*32)];
 }

 [[nodiscard]] constexpr std::array<std::uint8_t, 16> read_background_pattern(unsigned int pattern_index) noexcept {
   std::array<std::uint8_t, 16> pattern{};

   // std::memcpy is not constexpr :(
   for(unsigned int i = 0; i < 16; ++i) {
     pattern[i] = m_memory[m_regs.ppuctrl_background_pattern_base_address() + pattern_index + i];
   }

   return pattern;
 }

 int ctr = 0;
 constexpr void write8(std::uint16_t address, std::uint8_t value) noexcept {
   if(address >= 0x2000 && address < 0x3000){
     ctr++;
   }
   if(address >= 0 && address < 0x2000) {
     ctr--;
   }
   m_memory[address] = value;
 }
 private:
  std::array<std::uint8_t, 0x4000> m_memory;
 ppu_registers& m_regs;
  std::uint16_t m_nametable_base_address;
  std::uint16_t m_vram_address_increment;
  std::uint16_t m_sprite_pattern_table_address;
  std::uint16_t m_bg_pattern_table_address;
  bool m_generate_vblank_nmi;
};

#endif  // NES_VRAM_CONTROLLER_H

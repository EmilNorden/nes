#ifndef NES_PPU_H
#define NES_PPU_H

#include <cstdint>

class ppu {
 public:
  ppu() : m_odd_frame(false), m_current_scanline(-1), m_scanline_cycle(0) {}

  constexpr void process(int cycles) noexcept {
    if(m_current_scanline >= -1 && m_current_scanline < 240) // dummy and visible scanline
    {

    }
  }

 private:
  int m_current_scanline;
  int m_cycle_remainders;
  int m_scanline_cycle;
  std::uint16_t m_pattern_table1;
  std::uint16_t m_pattern_table2;
  std::uint8_t m_palette_attribs1;
  std::uint8_t m_palette_attribs2;
  std::uint8_t m_memory[0x4000];
  bool m_odd_frame;
};

#endif  // NES_PPU_H

#ifndef NES_PPU_H
#define NES_PPU_H

#include <array>
#include <cstdint>
#include "ppu_registers.h"
#include "vram_controller.h"

enum class fetch_state { nametable, attribute, pattern_low, pattern_high };

class ppu2c02 {
 public:
  explicit ppu2c02(vram_controller& memory, ppu_registers& ppu_regs)
      : m_memory(memory),
        m_ppu_regs(ppu_regs),
        m_fetch_state(fetch_state::nametable),
        m_tile_x(0),
        m_tile_y(0),
        m_current_scanline(-1),
        m_scanline_cycle(0),
        m_odd_frame(false) {}

  constexpr bool spend_cycles(int cycle_count) noexcept {
    if (cycle_count <= m_cycle_remainders) {
      m_cycle_remainders -= cycle_count;
      m_scanline_cycle += cycle_count;
      return true;
    }

    return false;
  }

  /*constexpr void process(int ppu_cycles, unsigned int* pixels) noexcept {
    m_cycle_remainders += ppu_cycles;

    if (m_cycle_remainders >= (241 * 341)) {
      m_cycle_remainders -= (241 * 341);
      m_ppu_regs.set_ppustatus_vblank();
    }

    for (unsigned int y = 0; y < 240; ++y) {
      for (unsigned int x = 0; x < 256; ++x) {
        unsigned int tilex = x / 8U;
        unsigned int tiley = y / 8U;
        unsigned int localx = x % 8U;
        unsigned int localy = y % 8U;

        auto nametable_byte = m_memory.read_nametable(tilex, tiley);
        auto pattern = m_memory.read_background_pattern(nametable_byte);

        unsigned int pattern_low = pattern[localy] >> (7U - localx);
        unsigned int pattern_high = pattern[localy + 8U] >> (7U - localx);
        if (pattern_low | pattern_high) {
          pixels[(y * 256) + x] = 0xFF0000FF;
        } else {
          pixels[(y * 256) + x] = 0xFF000000;
        }
      }
    }
  }*/

  constexpr void process(int ppu_cycles, unsigned int* pixels) noexcept {
    auto foo = m_memory.read_nametable(0, 0);

    if(foo != 0) {
      return;
    }
  }

  constexpr void process2(int ppu_cycles, unsigned int* pixels) noexcept {
    m_cycle_remainders += ppu_cycles;

    if(m_current_scanline == -1) {
      // dummy scanline
      // purpose: fill shift registers with data for first two tiles of next scanline
      // same memory accesses as regular scanlines
    }

    // Varje scanline tar 341 PPU cycles (113.667 CPU cycles)
    // Varje cycle producerar 1 pixel

    // First cycle of scanline is idle.
    if(m_scanline_cycle == 0) {
      spend_cycles(1);
    }

    if (m_fetch_state == fetch_state::nametable && spend_cycles(2)) {
      m_nametable_byte = m_memory.read_nametable(m_tile_x, m_tile_y);
      m_fetch_state = fetch_state::attribute;
    }
    if (m_fetch_state == fetch_state::attribute && spend_cycles(2)) {
      m_attribute_byte = 0;  // TODO: READ ATTRIBUTE BYTE
      m_fetch_state = fetch_state::pattern_low;
    }
    if (m_fetch_state == fetch_state::pattern_low && spend_cycles(2)) {
      m_pattern_table_low = 0;  // TODO: READ PATTERN TABLE LOW
      auto pattern = m_memory.read_background_pattern(m_tile_x);

      m_fetch_state = fetch_state::pattern_high;
    }
    if (m_fetch_state == fetch_state::pattern_high && spend_cycles(2)) {
      m_pattern_table_high = 0;  // TODO: READ PATTERN TABLE HIGH
      m_fetch_state = fetch_state::nametable;
    }

    /*if(m_current_scanline >= -1 && m_current_scanline < 240) // dummy and
    visible scanline
    {

    }*/
  }

 private:
  // Frame is 256x240 pixels. Each tile is 8x8 pixels. That means 32x30 tiles :)
  std::array<std::uint8_t, 256> m_scanline_pixels;
  vram_controller& m_memory;
  ppu_registers& m_ppu_regs;
  fetch_state m_fetch_state;
  unsigned int m_tile_x, m_tile_y;
  int m_current_scanline;
  int m_cycle_remainders;
  int m_scanline_cycle;
  std::uint16_t m_nametable_base_address;
  std::uint8_t m_nametable_byte;
  std::uint8_t m_attribute_byte;
  std::uint8_t m_pattern_table_low;
  std::uint8_t m_pattern_table_high;
  std::uint8_t m_palette_attribs1;
  std::uint8_t m_palette_attribs2;

  // These two uint8_t's represent the 16-bit shift register for pattern data
  std::uint8_t m_current_tile_pattern_data;
  std::uint8_t m_next_tile_pattern_data;
  bool m_odd_frame;
};

#endif  // NES_PPU_H

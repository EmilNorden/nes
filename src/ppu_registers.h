#ifndef NES_PPU_REGISTERS_H
#define NES_PPU_REGISTERS_H

#include <cstdint>

class ppu_registers {
 private:
  std::uint16_t m_ppuaddr;
  std::uint8_t m_ppuctrl;
  std::uint8_t m_ppumask;
  std::uint8_t m_oamaddr;
  std::uint8_t m_oamdata;
  std::uint8_t m_ppuscroll_x;
  std::uint8_t m_ppuscroll_y;
  std::uint8_t m_ppustatus;
  bool m_ppuscroll_toggle;
  bool m_ppuaddr_toggle;

 public:
  [[nodiscard]] constexpr std::uint16_t ppuctrl_nametable_base_address() noexcept {
    auto nametable_index = m_ppuctrl & 0x03U;
    return static_cast<std::uint16_t>(0x2000U + (nametable_index * 0x400U));
  }

  [[nodiscard]] constexpr std::uint16_t ppuctrl_background_pattern_base_address() noexcept {
    return (m_ppuctrl & 0x10) ? 0x1000 : 0x0000;
  }

  /*constexpr*/ void set_ppuctrl(std::uint8_t value) noexcept {
    m_ppuctrl = value;
  }

  /*constexpr*/ void set_ppumask(std::uint8_t value) noexcept {
    m_ppumask = value;
  }

  /*constexpr*/ void set_oamaddr(std::uint8_t value) noexcept {
    m_oamaddr = value;
  }

  /*constexpr*/ void set_oamdata(std::uint8_t value) noexcept {
    m_oamdata = value;
  }

  /*constexpr*/ void set_ppuscroll(std::uint8_t value) noexcept {
    if (m_ppuscroll_toggle) {
      m_ppuscroll_y = value;
    } else {
      m_ppuscroll_x = value;
    }

    m_ppuscroll_toggle = !m_ppuscroll_toggle;
  }

  /*constexpr*/ void set_ppuaddr(std::uint8_t value) noexcept {
    if (m_ppuaddr_toggle) {
      m_ppuaddr |= value;
    } else {
      m_ppuaddr = static_cast<std::uint16_t>(value << 8U);
    }

    m_ppuaddr_toggle = !m_ppuaddr_toggle;
  }

  [[nodiscard]] constexpr std::uint16_t ppuaddr() noexcept {
    return m_ppuaddr;
  }

  [[nodiscard]] constexpr std::uint8_t ppustatus() noexcept {
    auto result =  m_ppustatus;
    m_ppustatus &= ~0x80U;

    return result;
  }

  constexpr void set_ppustatus_vblank() noexcept {
    m_ppustatus |= 0x80U;
  }

  constexpr void increment_ppuaddr() noexcept {
    if(m_ppuctrl & 0x4) {
      m_ppuaddr += 0x20;
    }
    else {
      m_ppuaddr += 0x1;
    }
  }
  // TODO: PPUSTATUS
};

#endif  // NES_PPU_REGISTERS_H

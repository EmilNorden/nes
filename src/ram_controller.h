#ifndef NES_RAM_CONTROLLER_H
#define NES_RAM_CONTROLLER_H

#include <array>
#include <cstddef>
#include <cstring>
#include <functional>
#include "common.h"
#include "ppu_registers.h"
#include "prg_rom_bank.h"
#include "vram_controller.h"

class ram_controller {
 private:
  ppu_registers& m_ppu_registers;
  vram_controller &m_vram;
  std::array<std::uint8_t, 0xFFFF> m_memory;

  // Returns true if address is within the range $0000-$1FFF
  [[nodiscard]] constexpr auto is_lower_ram_range(std::uint16_t address) const
      noexcept {
    return (address & 0x1FFFU) == address;
  }

  [[nodiscard]] constexpr auto is_io_mirror_range(std::uint16_t address) const
      noexcept {
    return (address & 0x3FFFU) == address;
  }

  [[nodiscard]] constexpr auto translate_address(std::uint16_t address) const
      noexcept {
    if (is_lower_ram_range(address)) {
      // Address range $0000-$07FF is mirrored 3 times
      address = address & 0x07FFU;
    } else if (is_io_mirror_range((address))) {
      // Address range $2000-$2007 is mirrored multiple times

      address = address & 0x2007U;
    }

    return address;
  }

 public:
  constexpr ram_controller(ppu_registers& ppu_regs, vram_controller& vram)
      : m_ppu_registers(ppu_regs), m_vram(vram), m_memory() {}

  [[nodiscard]] constexpr auto read8(std::uint16_t address) const noexcept {
    address = translate_address(address);

    if(address == 0x2002) {
      return (std::uint8_t)0x80;
      // return m_ppu_registers.ppustatus();
    }

    return m_memory[address];
  }
  [[nodiscard]] constexpr auto read16(std::uint16_t address) const noexcept {
    // I will assume that we will never attempt to read 16bit that crosses the
    // border of two ranges i.e the range that (address) occupies is not the
    // same as (address+1). Otherwise I would have to do
    // translate_address(address) AND translate_address(address+1)
    address = translate_address(address);

    return static_cast<std::uint16_t>(
        m_memory[address] |
        static_cast<std::uint16_t>(m_memory[address + 1] << 8U));
  }

  /*constexpr*/ void handle_ppu_registers(std::uint16_t address,
                                          std::uint8_t value) noexcept {
    switch (address) {
      case 0x2000:
        m_ppu_registers.set_ppuctrl(value);
        break;
      case 0x2001:
        m_ppu_registers.set_ppumask(value);
        break;
      case 0x2003:
        m_ppu_registers.set_oamaddr(value);
        break;
      case 0x2004:
        m_ppu_registers.set_oamdata(value);
        break;
      case 0x2005:
        m_ppu_registers.set_ppuscroll(value);
        break;
      case 0x2006:
        m_ppu_registers.set_ppuaddr(value);
        break;
      case 0x2007:
        m_vram.write8(m_ppu_registers.ppuaddr(), value);
        m_ppu_registers.increment_ppuaddr();
        break;
      case 0x4014:
        m_ppu_registers.set_ppuaddr(0);
        break;
      default:
        break;
    }
  }

  /*constexpr*/ void write8(std::uint16_t address,
                            std::uint8_t value) noexcept {
    address = translate_address(address);

    // std::cerr << fmt::format("{:04X} <-- {:02X}\n", address, value);

    handle_ppu_registers(address, value);

    m_memory[address] = value;
  }

  void load_prg_bank1(const prg_rom_bank& rom) {
    std::memcpy(&m_memory[0x8000], rom.value().data(), 0x4000);
  }

  void load_prg_bank2(const prg_rom_bank& rom) {
    std::memcpy(&m_memory[0xC000], rom.value().data(), 0x4000);
  }
};

#endif  // NES_RAM_CONTROLLER_H

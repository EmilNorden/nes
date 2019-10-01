#ifndef NES_RAM_CONTROLLER_H
#define NES_RAM_CONTROLLER_H

#include <cstddef>
#include <cstring>
#include "common.h"
#include "prg_rom_bank.h"

class ram_controller {
 private:
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

  std::uint8_t m_memory[0xFFFF];

 public:
  [[nodiscard]] constexpr auto read8(std::uint16_t address) const noexcept {
    return m_memory[translate_address(address)];
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

  constexpr void write8(std::uint16_t address, std::uint8_t value) noexcept {
      address = translate_address(address);

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

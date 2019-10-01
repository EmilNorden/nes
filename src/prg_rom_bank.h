#ifndef NES_PRG_ROM_BANK_H
#define NES_PRG_ROM_BANK_H

#include <array>
#include <cstdint>

class prg_rom_bank {
 public:
  explicit prg_rom_bank(std::array<std::uint8_t, 0x4000>& data)
      : m_data(std::move(data)) {}

  [[nodiscard]] constexpr const auto& value() const noexcept { return m_data; }

 private:
  std::array<std::uint8_t, 0x4000> m_data;
};

#endif  // NES_PRG_ROM_BANK_H

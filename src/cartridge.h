#ifndef NES_CARTRIDGE_H
#define NES_CARTRIDGE_H

#include <vector>
#include "prg_rom_bank.h"

class cartridge {
 public:
  explicit cartridge(std::vector<prg_rom_bank>& prg_rom)
      : m_prg_rom(std::move(prg_rom)) {}

  constexpr const auto& prg_rom() const noexcept { return m_prg_rom; }

 private:
  std::vector<prg_rom_bank> m_prg_rom;
};

#endif  // NES_CARTRIDGE_H

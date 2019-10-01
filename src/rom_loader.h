//
// Created by Emil Nordén on 2019-09-20.
//

#ifndef NES_ROM_LOADER_H
#define NES_ROM_LOADER_H

#include <istream>
#include "cartridge.h"
#include <string>
#include <vector>

[[nodiscard]] cartridge load_rom(std::istream& stream) noexcept {
  char header[16];
  stream.read(header, 16);

  std::string constant{header,3};
  if(constant != "NES") {
    std::cerr << "NES header not found!\n";
    abort();
  }

  int prg_rom_bank_count = header[4];
  int chr_rom_size = header[5] * 0x2000;
  auto flags6 = static_cast<std::uint8_t>(header[6]);

  const auto has_traniner_mask = 0b00000100;

  if((flags6 & has_traniner_mask) == has_traniner_mask) {
    // There are 512 bytes of trainer data before the prg rom, so we skip
    // past is for now.
    stream.seekg(512, std::ios_base::cur);
  }

  auto g = stream.tellg();

  std::vector<prg_rom_bank> prg_rom_banks;
  for(auto i = 0; i < prg_rom_bank_count; ++i) {
    std::array<char, 0x4000> buffer;
    std::array<std::uint8_t, 0x4000> rom_bank;

    stream.read(buffer.data(), buffer.size());
    std::memcpy(rom_bank.data(), buffer.data(), rom_bank.size());
    prg_rom_banks.emplace_back(rom_bank);
    // I could have done away with the buffer vector and just reinterpret_cast'ed rom_bank in the stream.read() call,
    // but i am trying to avoid using reinterpret_cast here (after listening to Jason Turners advice)
  }

  return cartridge{prg_rom_banks};
}

#endif  // NES_ROM_LOADER_H

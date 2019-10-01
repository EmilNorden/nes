#include <fstream>
#include "cpu.h"
#include "fmt/format.h"
#include "rom_loader.h"

int main() {
  std::ifstream f{"../../roms/nestest.nes", std::ios::binary};

  auto a = load_rom(f);

  ram_controller ram{};
  cpu2a03 cpu{ram};

  if (a.prg_rom().size() > 1) {
    ram.load_prg_bank1(a.prg_rom()[0]);
    ram.load_prg_bank2(a.prg_rom()[1]);
  } else {
    ram.load_prg_bank1(a.prg_rom()[0]);
    ram.load_prg_bank2(a.prg_rom()[0]);
  }

  cpu.reset();
  int iterations = 0;
  int total_cycles = 0;
  while (true) {
    auto regs = cpu.m_registers;
    auto cycles = cpu.process_instruction();
    std::cout << fmt::format(
        "A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP: {:02X} CYC: {:>3}",
        regs.accumulator(), regs.x(), regs.y(), regs.status(),
        regs.stack() & 0xFFU, total_cycles);

    total_cycles += cycles *3;
    std::cout << std::endl;

    if(++iterations > 10000) {
      break;
    }
  }
}
#ifndef NES_OPCODES_H
#define NES_OPCODES_H

#include <cstddef>
#include <iostream>
#include "cpu_registers.h"
#include "fmt/format.h"
#include "ram_controller.h"

template <unsigned int BitNum>
[[nodiscard]] /*constexpr*/ auto bitmask() noexcept {
  return (1U << BitNum);
}

void push_stack(cpu_registers& regs, ram_controller& mem, std::uint8_t val) {
  mem.write8(regs.stack(), val);
  regs.decrement_stack();
}

[[nodiscard]] /*constexpr*/ auto pop_stack(cpu_registers& regs,
                                           const ram_controller& mem) noexcept {
  regs.increment_stack();
  return mem.read8(regs.stack());
}

namespace mode {

struct addressing_result {
  std::uint16_t address;
  bool page_boundary_crossed;
};

[[nodiscard]] /*constexpr*/ auto immediate(cpu_registers& regs,
                                           const ram_controller& mem) noexcept {
  auto address = regs.increment_pc();
  std::cout << fmt::format("{:02X}      ", mem.read8(address));
  return address;
}

[[nodiscard]] /*constexpr*/ auto immediate_read(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  return mem.read8(mode::immediate(regs, mem));
}

[[nodiscard]] /*constexpr*/ auto absolute(cpu_registers& regs,
                                          const ram_controller& mem) noexcept {
  auto low = mem.read8(regs.increment_pc());
  auto high = mem.read8(regs.increment_pc());
  std::cout << fmt::format("{:02X} {:02X}   ", low, high);
  return static_cast<std::uint16_t>(low |
                                    static_cast<std::uint16_t>(high << 8U));
}

[[nodiscard]] /*constexpr*/ auto absolute_read(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  return mem.read8(mode::absolute(regs, mem));
}

[[nodiscard]] /*constexpr*/ auto absolute_indexed(cpu_registers& regs,
                                                  const ram_controller& mem,
                                                  std::uint8_t index) noexcept {
  addressing_result result{};
  auto low = mem.read8(regs.increment_pc());
  auto high = mem.read8(regs.increment_pc());

  std::cout << fmt::format("{:02X} {:02X}   ", low, high);

  low = low + index;
  if (low < index) {
    result.page_boundary_crossed = true;
    high += 1;
  }

  result.address =
      static_cast<std::uint16_t>(low | static_cast<std::uint16_t>(high << 8U));
  return result;
}

[[nodiscard]] /*constexpr*/ auto zero_page(cpu_registers& regs,
                                           const ram_controller& mem) noexcept {
  auto low = mem.read8(regs.increment_pc());
  std::cout << fmt::format("{:02X}      ", low);
  return static_cast<std::uint16_t>(low);
}

[[nodiscard]] /*constexpr*/ auto zero_page_read(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  return mem.read8(mode::zero_page(regs, mem));
}

[[nodiscard]] /*constexpr*/ auto zero_page_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto zero_page_addr = mem.read8(regs.increment_pc());
  std::cout << fmt::format("{:02X}      ", zero_page_addr);
  return static_cast<std::uint16_t>(
      static_cast<std::uint8_t>(zero_page_addr + regs.x()));
}

[[nodiscard]] /*constexpr*/ auto zero_page_x_read(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  return mem.read8(mode::zero_page_x(regs, mem));
}

[[nodiscard]] /*constexpr*/ auto zero_page_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto zero_page_addr = mem.read8(regs.increment_pc());
  std::cout << fmt::format("{:02X}      ", zero_page_addr);
  return static_cast<std::uint16_t>(
      static_cast<std::uint8_t>(zero_page_addr + regs.y()));
}

[[nodiscard]] /*constexpr*/ auto zero_page_y_read(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  return mem.read8(mode::zero_page_y(regs, mem));
}

[[nodiscard]] /*constexpr*/ auto relative(cpu_registers& regs,
                                          const ram_controller& mem) noexcept {
  auto low = mem.read8(regs.increment_pc());
  std::cout << fmt::format("{:02X}      ", low);
  return static_cast<std::int8_t>(low);
}

[[nodiscard]] /*constexpr*/ auto indirect(cpu_registers& regs,
                                          const ram_controller& mem) noexcept {
  auto low = mem.read8(regs.increment_pc());
  auto high = mem.read8(regs.increment_pc());

  std::cout << fmt::format("{:02X} {:02X}   ", low, high);

  auto address =
      static_cast<std::uint16_t>(low | static_cast<std::uint16_t>(high << 8U));

  // from documentation at obelisk.me.uk:
  // NB:
  // An original 6502 has does not correctly fetch the target address if the
  // indirect vector falls on a page boundary (e.g. $xxFF where xx is any value
  // from $00 to $FF). In this case fetches the LSB from $xxFF as expected but
  // takes the MSB from $xx00. This is fixed in some later chips like the 65SC02
  // so for compatibility always ensure the indirect vector is not at the end of
  // the page.
  auto address_high =
      static_cast<std::uint16_t>(low == 0xFF ? address & 0xFF00U : address + 1);

  return static_cast<std::uint16_t>(
      mem.read8(address) |
      static_cast<std::uint16_t>(mem.read8(address_high) << 8U));
}

[[nodiscard]] /*constexpr*/ auto indexed_indirect(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto low = mem.read8(regs.increment_pc());
  auto zero_page_addr = static_cast<std::uint8_t>(low + regs.x());

  std::cout << fmt::format("{:02X}      ", low);

  return static_cast<std::uint16_t>(
      mem.read8(zero_page_addr) |
      static_cast<std::uint16_t>(
          mem.read8(static_cast<std::uint8_t>(zero_page_addr + 1U)) << 8U));
}

[[nodiscard]] /*constexpr*/ auto indirect_indexed(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  addressing_result result{};
  auto zero_page_addr = mem.read8(regs.increment_pc());

  std::cout << fmt::format("{:02X}      ", zero_page_addr);

  auto low = mem.read8(zero_page_addr);
  auto high = mem.read8(static_cast<std::uint8_t>(zero_page_addr + 1U));

  low = low + regs.y();
  if (low < regs.y()) {
    result.page_boundary_crossed = true;
    high += 1;
  }

  result.address =
      static_cast<std::uint16_t>(low | static_cast<unsigned>(high << 8U));

  return result;
}

}  // namespace mode

namespace opcode {

[[nodiscard]] /*constexpr*/ int brk_implied(cpu_registers& regs,
                                            ram_controller& mem) noexcept {
  // Write program counter and status flag to stack (TODO Make this code more
  // expressive?)
  push_stack(regs, mem, regs.pc() & 0xFFU);
  push_stack(regs, mem,
             static_cast<std::uint16_t>(regs.pc() >> 8U) &
                 static_cast<std::uint16_t>(0xFF));

  // From the nesdev wiki:
  // In the byte pushed, bit 5 is always set to 1, and bit 4 is 1 if from an
  // instruction (PHP or BRK) or 0 if from an interrupt line being pulled low
  // (/IRQ or /NMI)
  push_stack(regs, mem,
             regs.status() | static_cast<std::uint8_t>(cpu_flag::unused) |
                 static_cast<std::uint8_t>(cpu_flag::break_command));

  regs.set_flag(cpu_flag::break_command);

  // set PC to address at IRQ interrupt vector 0xFFFE
  regs.set_pc(mem.read16(0xFFFE));

  return 7;
}

[[nodiscard]] /*constexpr*/ int php_implied(cpu_registers& regs,
                                            ram_controller& mem) noexcept {
  std::cout << "        ";
  // From the nesdev wiki:
  // In the byte pushed, bit 5 is always set to 1, and bit 4 is 1 if from an
  // instruction (PHP or BRK) or 0 if from an interrupt line being pulled low
  // (/IRQ or /NMI)
  push_stack(regs, mem,
             regs.status() | static_cast<std::uint8_t>(cpu_flag::unused) |
                 static_cast<std::uint8_t>(cpu_flag::break_command));
  return 3;
}

[[nodiscard]] /*constexpr*/ int plp_implied(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  std::cout << "        ";
  // PLP ignores bit 4 and 5. 5 is unused and should always be 1.
  auto status = pop_stack(regs, mem);
  status = status | static_cast<std::uint8_t>(cpu_flag::unused);
  status = status & static_cast<std::uint8_t>(
                        ~(static_cast<std::uint8_t>(cpu_flag::break_command)));
  regs.set_status(status);
  return 4;
}

/*constexpr*/ void ora(cpu_registers& regs, std::uint8_t value) noexcept {
  regs.set_accumulator(regs.accumulator() | value);
}

[[nodiscard]] /*constexpr*/ int ora_immediate(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  ora(regs, mode::immediate_read(regs, mem));
  return 2;
}

[[nodiscard]] /*constexpr*/ int ora_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  ora(regs, mode::zero_page_read(regs, mem));
  return 3;
}

[[nodiscard]] /*constexpr*/ int ora_zero_page_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  ora(regs, mode::zero_page_x_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int ora_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  ora(regs, mode::absolute_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int ora_absolute_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  ora(regs, mem.read8(result.address));
  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int ora_absolute_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.y());
  ora(regs, mem.read8(result.address));
  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int ora_indirect_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  ora(regs, mem.read8(mode::indexed_indirect(regs, mem)));
  return 6;
}

[[nodiscard]] /*constexpr*/ int ora_indirect_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto addressing = mode::indirect_indexed(regs, mem);
  ora(regs, mem.read8(addressing.address));
  return addressing.page_boundary_crossed ? 6 : 5;
}

/*constexpr*/ void asl(cpu_registers& regs,
                       ram_controller& mem,
                       std::uint16_t address) noexcept {
  auto old_value = mem.read8(address);
  auto new_value = static_cast<std::uint8_t>(old_value << 1U);

  regs.set_flag_if(cpu_flag::carry, (old_value & 0x80U) == 0x80U);
  regs.set_flag_if(cpu_flag::zero, new_value == 0);
  regs.set_flag_if(cpu_flag::sign, (new_value & 0x80U) == 0x80U);

  mem.write8(address, new_value);
}

[[nodiscard]] /*constexpr*/ int asl_accumulator(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_flag_if(cpu_flag::carry, (regs.accumulator() & 0x80U) == 0x80U);
  regs.set_accumulator(static_cast<std::uint8_t>(regs.accumulator() << 1U));
  // asl(regs, regs.accumulator());
  return 2;
}

[[nodiscard]] /*constexpr*/ int asl_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  asl(regs, mem, mode::zero_page(regs, mem));

  return 5;
}

[[nodiscard]] /*constexpr*/ int asl_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  asl(regs, mem, mode::zero_page_x(regs, mem));

  return 6;
}

[[nodiscard]] /*constexpr*/ int asl_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  asl(regs, mem, mode::absolute(regs, mem));

  return 6;
}

[[nodiscard]] /*constexpr*/ int asl_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  // TODO: Why doesnt this instruction use an extra cycle for page boundary
  // crosses?
  asl(regs, mem, result.address);

  return 7;
}

[[nodiscard]] /*constexpr*/ int pla_implied(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  std::cout << "        ";
  regs.set_accumulator(pop_stack(regs, mem));

  return 4;
}

[[nodiscard]] /*constexpr*/ int rts_implied(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  std::cout << "        ";
  auto low = pop_stack(regs, mem);
  auto high = pop_stack(regs, mem) << 8U;
  auto address =
      static_cast<std::uint16_t>(low | static_cast<std::uint16_t>(high)) + 1U;
  regs.set_pc(address);

  return 6;
}

/*constexpr*/ void adc(cpu_registers& regs, std::uint8_t value) {
  auto result = regs.accumulator() + value +
                (regs.status() & static_cast<unsigned>(cpu_flag::carry));

  // If we wrapped around, set carry flag
  regs.set_flag_if(cpu_flag::carry, result > 0xFF);

  // Set overflow if sign bit is incorrect
  // That is, if the numbers added have identical signs, but the sign of the
  // result differs, set overflow.
  regs.set_flag_if(cpu_flag::overflow,
                   static_cast<std::uint8_t>(
                       static_cast<std::uint8_t>(~static_cast<std::uint8_t>(
                           regs.accumulator() ^ value)) &
                       static_cast<std::uint8_t>(regs.accumulator() ^ result)) &
                       0x80U);
  regs.set_accumulator(result & 0xFFU);
}

[[nodiscard]] /*constexpr*/ int adc_immediate(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  adc(regs, mode::immediate_read(regs, mem));

  return 2;
}

[[nodiscard]] /*constexpr*/ int adc_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  adc(regs, mode::zero_page_read(regs, mem));

  return 3;
}

[[nodiscard]] /*constexpr*/ int adc_zero_page_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  adc(regs, mode::zero_page_x_read(regs, mem));

  return 4;
}

[[nodiscard]] /*constexpr*/ int adc_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  adc(regs, mode::absolute_read(regs, mem));

  return 4;
}

[[nodiscard]] /*constexpr*/ int adc_absolute_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());

  adc(regs, mem.read8(result.address));

  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int adc_absolute_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.y());

  adc(regs, mem.read8(result.address));

  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int adc_indirect_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  adc(regs, mem.read8(mode::indexed_indirect(regs, mem)));

  return 6;
}

[[nodiscard]] /*constexpr*/ int adc_indirect_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto addressing = mode::indirect_indexed(regs, mem);
  adc(regs, mem.read8(addressing.address));

  return addressing.page_boundary_crossed ? 6 : 5;
}

[[nodiscard]] /*constexpr*/ int sei_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_flag(cpu_flag::interrupt_disable);
  return 2;
}

[[nodiscard]] /*constexpr*/ int bcs_relative(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto relative_address = mode::relative(regs, mem);
  if (regs.flag(cpu_flag::carry)) {
    if (regs.offset_pc(relative_address)) {
      return 4;
    }
    return 3;
  }
  return 2;
}

[[nodiscard]] /*constexpr*/ int bne_relative(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto relative_address = mode::relative(regs, mem);
  if (!regs.flag(cpu_flag::zero)) {
    if (regs.offset_pc(relative_address)) {
      return 4;
    }
    return 3;
  }
  return 2;
}

[[nodiscard]] /*constexpr*/ int cld_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.clear_flag(cpu_flag::clear_decimal_mode);
  return 2;
}

[[nodiscard]] /*constexpr*/ int ldy_immediate(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_y(mode::immediate_read(regs, mem));
  return 2;
}

[[nodiscard]] /*constexpr*/ int ldy_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_y(mode::zero_page_read(regs, mem));
  return 3;
}

[[nodiscard]] /*constexpr*/ int ldy_zero_page_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_y(mode::zero_page_x_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int ldy_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_y(mode::absolute_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int ldy_absolute_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  regs.set_y(mem.read8(result.address));
  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int ldx_immediate(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_x(mode::immediate_read(regs, mem));
  return 2;
}

[[nodiscard]] /*constexpr*/ int ldx_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_x(mode::zero_page_read(regs, mem));
  return 3;
}

[[nodiscard]] /*constexpr*/ int ldx_zero_page_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_x(mode::zero_page_y_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int ldx_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_x(mode::absolute_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int ldx_absolute_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.y());
  regs.set_x(mem.read8(result.address));
  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int sta_indirect_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  mem.write8(mode::indexed_indirect(regs, mem), regs.accumulator());
  return 6;
}

[[nodiscard]] /*constexpr*/ int sta_indirect_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto addressing = mode::indirect_indexed(regs, mem);
  mem.write8(addressing.address, regs.accumulator());
  return 6;
}

[[nodiscard]] /*constexpr*/ int sta_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  mem.write8(mode::zero_page(regs, mem), regs.accumulator());
  return 3;
}

[[nodiscard]] /*constexpr*/ int sta_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  mem.write8(mode::zero_page_x(regs, mem), regs.accumulator());
  return 4;
}

[[nodiscard]] /*constexpr*/ int sta_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  mem.write8(mode::absolute(regs, mem), regs.accumulator());
  return 4;
}

[[nodiscard]] /*constexpr*/ int sta_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  mem.write8(result.address, regs.accumulator());
  return 5;
}

[[nodiscard]] /*constexpr*/ int sta_absolute_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.y());
  mem.write8(result.address, regs.accumulator());
  return 5;
}

[[nodiscard]] /*constexpr*/ int stx_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  mem.write8(mode::zero_page(regs, mem), regs.x());
  return 3;
}

[[nodiscard]] /*constexpr*/ int stx_zero_page_y(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  mem.write8(mode::zero_page_y(regs, mem), regs.x());
  return 4;
}

[[nodiscard]] /*constexpr*/ int stx_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  mem.write8(mode::absolute(regs, mem), regs.x());
  return 4;
}

[[nodiscard]] /*constexpr*/ int sty_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  mem.write8(mode::zero_page(regs, mem), regs.y());
  return 3;
}

[[nodiscard]] /*constexpr*/ int sty_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  mem.write8(mode::zero_page_x(regs, mem), regs.y());
  return 4;
}

[[nodiscard]] /*constexpr*/ int sty_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  mem.write8(mode::absolute(regs, mem), regs.y());
  return 4;
}

[[nodiscard]] /*constexpr*/ int tay_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_y(regs.accumulator());
  return 2;
}

[[nodiscard]] /*constexpr*/ int tax_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_x(regs.accumulator());
  return 2;
}

[[nodiscard]] /*constexpr*/ int tsx_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_x(regs.stack() & 0xFFU);
  return 2;
}

[[nodiscard]] /*constexpr*/ int txa_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_accumulator(regs.x());
  return 2;
}

[[nodiscard]] /*constexpr*/ int tya_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_accumulator(regs.y());
  return 2;
}

[[nodiscard]] /*constexpr*/ int bcc_relative(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto relative_address = mode::relative(regs, mem);
  if (!regs.flag(cpu_flag::carry)) {
    if (regs.offset_pc(relative_address)) {
      return 4;
    }
    return 3;
  }
  return 2;
}

[[nodiscard]] /*constexpr*/ int txs_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_stack(regs.x());
  return 2;
}

[[nodiscard]] /*constexpr*/ int lax_indirect_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_accumulator(mem.read8(mode::indexed_indirect(regs, mem)));
  regs.set_x(regs.accumulator());
  return 6;
}

[[nodiscard]] /*constexpr*/ int lax_indirect_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto addressing = mode::indirect_indexed(regs, mem);
  regs.set_accumulator(mem.read8(addressing.address));
  regs.set_x(regs.accumulator());
  return addressing.page_boundary_crossed ? 6 : 5;
}

[[nodiscard]] /*constexpr*/ int lax_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_accumulator(mem.read8(mode::zero_page(regs, mem)));
  regs.set_x(regs.accumulator());
  return 3;
}

[[nodiscard]] /*constexpr*/ int lax_zero_page_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_accumulator(mem.read8(mode::zero_page_y(regs, mem)));
  regs.set_x(regs.accumulator());
  return 4;
}

[[nodiscard]] /*constexpr*/ int lax_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_accumulator(mem.read8(mode::absolute(regs, mem)));
  regs.set_x(regs.accumulator());
  return 4;
}

[[nodiscard]] /*constexpr*/ int lax_absolute_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto addressing = mode::absolute_indexed(regs, mem, regs.y());
  regs.set_accumulator(mem.read8(addressing.address));
  regs.set_x(regs.accumulator());
  return addressing.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int sax_indirect_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  mem.write8(mode::indexed_indirect(regs, mem), regs.accumulator() & regs.x());
  return 6;
}

[[nodiscard]] /*constexpr*/ int sax_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  mem.write8(mode::zero_page(regs, mem), regs.accumulator() & regs.x());
  return 6;
}

[[nodiscard]] /*constexpr*/ int sax_zero_page_y(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  mem.write8(mode::zero_page_y(regs, mem), regs.accumulator() & regs.x());
  return 4;
}

[[nodiscard]] /*constexpr*/ int sax_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  mem.write8(mode::absolute(regs, mem), regs.accumulator() & regs.x());
  return 4;
}

[[nodiscard]] /*constexpr*/ int lda_indirect_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_accumulator(mem.read8(mode::indexed_indirect(regs, mem)));
  return 6;
}

[[nodiscard]] /*constexpr*/ int lda_indirect_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto addressing = mode::indirect_indexed(regs, mem);
  regs.set_accumulator(mem.read8(addressing.address));
  return addressing.page_boundary_crossed ? 6 : 5;
}

[[nodiscard]] /*constexpr*/ int lda_immediate(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_accumulator(mode::immediate_read(regs, mem));
  return 2;
}

[[nodiscard]] /*constexpr*/ int lda_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_accumulator(mode::zero_page_read(regs, mem));
  return 3;
}

[[nodiscard]] /*constexpr*/ int lda_zero_page_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_accumulator(mode::zero_page_x_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int lda_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_accumulator(mode::absolute_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int lda_absolute_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  regs.set_accumulator(mem.read8(result.address));
  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int lda_absolute_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.y());
  regs.set_accumulator(mem.read8(result.address));
  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int bpl_relative(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto relative_address = mode::relative(regs, mem);
  if (!regs.flag(cpu_flag::sign)) {
    if (regs.offset_pc(relative_address)) {
      return 4;
    }
    return 3;
  }

  return 2;
}

[[nodiscard]] /*constexpr*/ int clc_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.clear_flag(cpu_flag::carry);
  return 2;
}

[[nodiscard]] /*constexpr*/ int jsr_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  auto routine_address = mode::absolute(regs, mem);
  auto return_address = regs.pc() - 1U;

  // I think high byte should be pushed first
  push_stack(regs, mem,
             static_cast<std::uint16_t>(return_address >> 8U) &
                 static_cast<std::uint16_t>(0xFFU));
  push_stack(regs, mem, return_address & 0xFFU);

  regs.set_pc(routine_address);

  return 6;
}

/*constexpr*/ void bit(cpu_registers& regs, std::uint8_t value) noexcept {
  if ((value & regs.accumulator()) == 0) {
    regs.set_flag(cpu_flag::zero);
  } else {
    regs.clear_flag(cpu_flag::zero);
  }

  if ((value & bitmask<6>()) == 0) {
    regs.clear_flag(cpu_flag::overflow);
  } else {
    regs.set_flag(cpu_flag::overflow);
  }

  if ((value & bitmask<7>()) == 0) {
    regs.clear_flag(cpu_flag::sign);
  } else {
    regs.set_flag(cpu_flag::sign);
  }
}

[[nodiscard]] /*constexpr*/ int bit_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  bit(regs, mode::zero_page_read(regs, mem));

  return 3;
}

[[nodiscard]] /*constexpr*/ int rol_accumulator(cpu_registers& regs) noexcept {
  std::cout << "        ";

  auto result = static_cast<std::uint8_t>(regs.accumulator() << 1U);
  if (regs.flag(cpu_flag::carry)) {
    result |= 0x01;
  }
  regs.set_flag_if(cpu_flag::carry, (regs.accumulator() & 0x80U) == 0x80U);

  regs.set_accumulator(result);
  return 2;
}

/*constexpr*/ void rol(cpu_registers& regs,
                       ram_controller& mem,
                       std::uint16_t address) noexcept {
  auto old_value = mem.read8(address);
  auto new_value = static_cast<std::uint8_t>(old_value << 1U);
  if (regs.flag(cpu_flag::carry)) {
    new_value |= 0x01;
  }

  regs.set_flag_if(cpu_flag::carry, (old_value & 0x80U) == 0x80U);
  regs.set_flag_if(cpu_flag::sign, (new_value & 0x80U) == 0x80U);

  mem.write8(address, new_value);
}

[[nodiscard]] /*constexpr*/ int rol_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  rol(regs, mem, mode::zero_page(regs, mem));

  return 5;
}

[[nodiscard]] /*constexpr*/ int rol_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  rol(regs, mem, mode::zero_page_x(regs, mem));

  return 6;
}

[[nodiscard]] /*constexpr*/ int rol_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  rol(regs, mem, mode::absolute(regs, mem));

  return 6;
}

[[nodiscard]] /*constexpr*/ int rol_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  rol(regs, mem, result.address);

  return 7;
}

[[nodiscard]] /*constexpr*/ int ror_accumulator(cpu_registers& regs) noexcept {
  std::cout << "        ";
  auto result = static_cast<std::uint8_t>(regs.accumulator() >> 1U);

  if (regs.flag(cpu_flag::carry)) {
    result |= 0x80U;
  }
  regs.set_flag_if(cpu_flag::carry, (regs.accumulator() & 1U) == 1U);

  regs.set_accumulator(result);
  return 2;
}

/*constexpr*/ void ror(cpu_registers& regs,
                       ram_controller& mem,
                       std::uint16_t address) noexcept {
  auto old_value = mem.read8(address);
  auto new_value = static_cast<std::uint8_t>(old_value >> 1U);
  if (regs.flag(cpu_flag::carry)) {
    new_value |= 0x80U;
  }

  regs.set_flag_if(cpu_flag::carry, (old_value & 1U) == 1U);
  regs.set_flag_if(cpu_flag::sign, (new_value & 0x80U) == 0x80U);

  mem.write8(address, new_value);
}
[[nodiscard]] /*constexpr*/ int ror_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  ror(regs, mem, mode::zero_page(regs, mem));

  return 5;
}

[[nodiscard]] /*constexpr*/ int ror_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  ror(regs, mem, mode::zero_page_x(regs, mem));

  return 6;
}

[[nodiscard]] /*constexpr*/ int ror_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  ror(regs, mem, mode::absolute(regs, mem));

  return 6;
}

[[nodiscard]] /*constexpr*/ int ror_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  ror(regs, mem, result.address);

  return 7;
}

[[nodiscard]] /*constexpr*/ int bit_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  bit(regs, mode::absolute_read(regs, mem));

  return 4;
}

/*constexpr*/ void and_instr(cpu_registers& regs,
                             const ram_controller& mem,
                             std::uint16_t address) noexcept {
  auto value = mem.read8(address);
  regs.set_accumulator(regs.accumulator() & value);
}

[[nodiscard]] /*constexpr*/ int and_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  and_instr(regs, mem, mode::zero_page(regs, mem));

  return 3;
}

[[nodiscard]] /*constexpr*/ int and_indirect_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  and_instr(regs, mem, mode::indexed_indirect(regs, mem));

  return 6;
}

[[nodiscard]] /*constexpr*/ int and_indirect_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto addressing = mode::indirect_indexed(regs, mem);
  and_instr(regs, mem, addressing.address);

  return addressing.page_boundary_crossed ? 6 : 5;
}

[[nodiscard]] /*constexpr*/ int and_immediate(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto value = mode::immediate_read(regs, mem);
  regs.set_accumulator(regs.accumulator() & value);

  return 2;
}

[[nodiscard]] /*constexpr*/ int and_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  and_instr(regs, mem, mode::absolute(regs, mem));

  return 4;
}

[[nodiscard]] /*constexpr*/ int and_zero_page_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  and_instr(regs, mem, mode::zero_page_x(regs, mem));

  return 4;
}

[[nodiscard]] /*constexpr*/ int and_absolute_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto addressing = mode::absolute_indexed(regs, mem, regs.x());
  and_instr(regs, mem, addressing.address);

  return addressing.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int and_absolute_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto addressing = mode::absolute_indexed(regs, mem, regs.y());
  and_instr(regs, mem, addressing.address);

  return addressing.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int bmi_relative(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto relative_address = mode::relative(regs, mem);
  if (regs.flag(cpu_flag::sign)) {
    if (regs.offset_pc(relative_address)) {
      return 4;
    }
    return 3;
  }
  return 2;
}

[[nodiscard]] /*constexpr*/ int sec_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_flag(cpu_flag::carry);
  return 2;
}

[[nodiscard]] /*constexpr*/ int pha_implied(cpu_registers& regs,
                                            ram_controller& mem) noexcept {
  std::cout << "        ";
  push_stack(regs, mem, regs.accumulator());

  return 3;
}

[[nodiscard]] /*constexpr*/ int lsr_accumulator(cpu_registers& regs) noexcept {
  std::cout << "        ";
  auto old_value = regs.accumulator();
  regs.set_flag_if(cpu_flag::carry, (old_value & 1U) == 1U);
  regs.set_accumulator(old_value >> 1U);

  return 2;
}

/*constexpr*/ void lsr(cpu_registers& regs,
                       ram_controller& mem,
                       std::uint16_t address) noexcept {
  auto old_value = mem.read8(address);
  auto new_value = static_cast<std::uint8_t>(old_value >> 1U);
  regs.set_flag_if(cpu_flag::carry, (old_value & 1U) == 1U);
  regs.set_flag_if(cpu_flag::zero, new_value == 0);
  regs.set_flag_if(cpu_flag::sign, (new_value & 0x80U) == 0x80U);

  mem.write8(address, old_value >> 1U);
}

[[nodiscard]] /*constexpr*/ int lsr_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  lsr(regs, mem, mode::zero_page(regs, mem));

  return 5;
}

[[nodiscard]] /*constexpr*/ int lsr_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  lsr(regs, mem, mode::zero_page_x(regs, mem));

  return 6;
}

[[nodiscard]] /*constexpr*/ int lsr_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  lsr(regs, mem, mode::absolute(regs, mem));

  return 6;
}

[[nodiscard]] /*constexpr*/ int lsr_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  lsr(regs, mem, result.address);

  return 7;
}

[[nodiscard]] /*constexpr*/ int jmp_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_pc(mode::absolute(regs, mem));

  return 3;
}

[[nodiscard]] /*constexpr*/ int jmp_indirect(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  regs.set_pc(mode::indirect(regs, mem));

  return 5;
}

[[nodiscard]] /*constexpr*/ int bvc_relative(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto relative_address = mode::relative(regs, mem);
  if (!regs.flag(cpu_flag::overflow)) {
    if (regs.offset_pc(relative_address)) {
      return 4;
    }
    return 3;
  }
  return 2;
}

[[nodiscard]] /*constexpr*/ int bvs_relative(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto relative_address = mode::relative(regs, mem);
  if (regs.flag(cpu_flag::overflow)) {
    if (regs.offset_pc(relative_address)) {
      return 4;
    }
    return 3;
  }
  return 2;
}

[[nodiscard]] /*constexpr*/ int cli_implied(cpu_registers& regs) noexcept {
  regs.clear_flag(cpu_flag::interrupt_disable);
  return 2;
}

[[nodiscard]] /*constexpr*/ int clv_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.clear_flag(cpu_flag::overflow);
  return 2;
}

/*constexpr*/ void cmp(cpu_registers& regs,
                       std::uint8_t register_value,
                       std::uint8_t value) noexcept {
  regs.set_flag_if(cpu_flag::carry, register_value >= value);
  regs.set_flag_if(cpu_flag::zero, register_value == value);

  auto result = static_cast<std::uint8_t>(register_value - value);
  regs.set_flag_if(cpu_flag::sign, (result & bitmask<7>()) == bitmask<7>());
}

[[nodiscard]] /*constexpr*/ int cmp_immediate(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  cmp(regs, regs.accumulator(), mode::immediate_read(regs, mem));
  return 2;
}

[[nodiscard]] /*constexpr*/ int cmp_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  cmp(regs, regs.accumulator(), mode::zero_page_read(regs, mem));
  return 3;
}

[[nodiscard]] /*constexpr*/ int cmp_zero_page_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  cmp(regs, regs.accumulator(), mode::zero_page_x_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int cmp_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  cmp(regs, regs.accumulator(), mode::absolute_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int cmp_absolute_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  cmp(regs, regs.accumulator(), mem.read8(result.address));

  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int cmp_absolute_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.y());
  cmp(regs, regs.accumulator(), mem.read8(result.address));

  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int cmp_indirect_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  cmp(regs, regs.accumulator(), mem.read8(mode::indexed_indirect(regs, mem)));
  return 6;
}

[[nodiscard]] /*constexpr*/ int cmp_indirect_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto addressing = mode::indirect_indexed(regs, mem);
  cmp(regs, regs.accumulator(), mem.read8(addressing.address));
  // TODO: Why does cmp() take accumulator? It can access it through 'regs'
  return addressing.page_boundary_crossed ? 6 : 5;
}

[[nodiscard]] /*constexpr*/ int cpx_immediate(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  cmp(regs, regs.x(), mode::immediate_read(regs, mem));

  return 2;
}

[[nodiscard]] /*constexpr*/ int cpx_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  cmp(regs, regs.x(), mode::zero_page_read(regs, mem));

  return 3;
}

[[nodiscard]] /*constexpr*/ int cpx_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  cmp(regs, regs.x(), mode::absolute_read(regs, mem));

  return 4;
}

[[nodiscard]] /*constexpr*/ int cpy_immediate(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  cmp(regs, regs.y(), mode::immediate_read(regs, mem));

  return 2;
}

[[nodiscard]] /*constexpr*/ int cpy_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  cmp(regs, regs.y(), mode::zero_page_read(regs, mem));

  return 3;
}

[[nodiscard]] /*constexpr*/ int cpy_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  cmp(regs, regs.y(), mode::absolute_read(regs, mem));

  return 4;
}

/*constexpr*/ void dec(cpu_registers& regs,
                       ram_controller& mem,
                       std::uint16_t address) noexcept {
  auto value = mem.read8(address);
  mem.write8(address, --value);

  regs.set_flag_if(cpu_flag::zero, value == 0);
  regs.set_flag_if(cpu_flag::sign, (value & bitmask<7>()) == bitmask<7>());
}

[[nodiscard]] /*constexpr*/ int dec_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  dec(regs, mem, mode::zero_page(regs, mem));
  return 5;
}

[[nodiscard]] /*constexpr*/ int dec_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  dec(regs, mem, mode::zero_page_x(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int dec_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  dec(regs, mem, mode::absolute(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int dec_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  // TODO Why no extra cycle for page boundary cross?
  dec(regs, mem, result.address);
  return 7;
}

[[nodiscard]] /*constexpr*/ int dex_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_x(regs.x() - 1U);
  return 2;
}

[[nodiscard]] /*constexpr*/ int dey_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_y(regs.y() - 1U);
  return 2;
}

/*constexpr*/ void eor(cpu_registers& regs, std::uint8_t value) noexcept {
  regs.set_accumulator(regs.accumulator() ^ value);
}

[[nodiscard]] /*constexpr*/ int eor_immediate(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  eor(regs, mode::immediate_read(regs, mem));
  return 2;
}

[[nodiscard]] /*constexpr*/ int eor_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  eor(regs, mode::zero_page_read(regs, mem));
  return 3;
}

[[nodiscard]] /*constexpr*/ int eor_zero_page_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  eor(regs, mode::zero_page_x_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int eor_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  eor(regs, mode::absolute_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int eor_absolute_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  eor(regs, mem.read8(result.address));
  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int eor_absolute_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.y());
  eor(regs, mem.read8(result.address));
  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int eor_indirect_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  eor(regs, mem.read8(mode::indexed_indirect(regs, mem)));
  return 6;
}

[[nodiscard]] /*constexpr*/ int eor_indirect_y(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto addressing = mode::indirect_indexed(regs, mem);
  eor(regs, mem.read8(addressing.address));
  return addressing.page_boundary_crossed ? 6 : 5;
}

[[nodiscard]] /*constexpr*/ int rti_implied(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  std::cout << "        ";
  regs.set_status(pop_stack(regs, mem) |
                  static_cast<std::uint8_t>(cpu_flag::unused));
  regs.set_pc(static_cast<std::uint16_t>(
      pop_stack(regs, mem) |
      static_cast<std::uint16_t>(pop_stack(regs, mem) << 8U)));

  return 3;
}

/*constexpr*/ void inc(cpu_registers& regs,
                       ram_controller& mem,
                       std::uint16_t address) noexcept {
  auto value = mem.read8(address);
  mem.write8(address, ++value);

  regs.set_flag_if(cpu_flag::zero, value == 0);
  regs.set_flag_if(cpu_flag::sign, (value & bitmask<7>()) == bitmask<7>());
}

[[nodiscard]] /*constexpr*/ int inc_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  inc(regs, mem, mode::zero_page(regs, mem));
  return 5;
}

[[nodiscard]] /*constexpr*/ int inc_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  inc(regs, mem, mode::zero_page_x(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int inc_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  inc(regs, mem, mode::absolute(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int inc_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  inc(regs, mem, result.address);
  return 7;
}

[[nodiscard]] /*constexpr*/ int inx_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_x(regs.x() + 1U);
  return 2;
}

[[nodiscard]] /*constexpr*/ int iny_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_y(regs.y() + 1U);
  return 2;
}

/*constexpr*/ void sbc(cpu_registers& regs, std::uint8_t value) noexcept {
  adc(regs, ~value);
  /*auto result = static_cast<std::uint8_t>(regs.accumulator() - value -
                                          (regs.flag(cpu_flag::carry) ? 0 : 1));

  // Set carry if 0 <= result <= 255, clear carry if underflow
  regs.set_flag_if(cpu_flag::carry, result < regs.accumulator());

  // Set overflow if sign bit is incorrect
  // That is, if the numbers added have identical signs, but the sign of the
  result differs, set overflow. regs.set_flag_if(cpu_flag::overflow,
                   static_cast<std::uint8_t>(
                       static_cast<std::uint8_t>(~static_cast<std::uint8_t>(
                           regs.accumulator() ^ value)) &
                       static_cast<std::uint8_t>(regs.accumulator() ^ result)) &
                   0x80U);

  regs.set_accumulator(result);*/
}

[[nodiscard]] /*constexpr*/ int sbc_immediate(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  sbc(regs, mode::immediate_read(regs, mem));
  return 2;
}

[[nodiscard]] /*constexpr*/ int sbc_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  sbc(regs, mode::zero_page_read(regs, mem));
  return 3;
}

[[nodiscard]] /*constexpr*/ int sbc_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  sbc(regs, mode::zero_page_x_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int sbc_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  sbc(regs, mode::absolute_read(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int sbc_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  sbc(regs, mem.read8(result.address));
  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int sbc_absolute_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.y());
  sbc(regs, mem.read8(result.address));
  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int sbc_indirect_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  sbc(regs, mem.read8(mode::indexed_indirect(regs, mem)));
  return 6;
}

[[nodiscard]] /*constexpr*/ int sbc_indirect_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto addressing = mode::indirect_indexed(regs, mem);
  sbc(regs, mem.read8(addressing.address));
  return addressing.page_boundary_crossed ? 6 : 5;
}

[[nodiscard]] /*constexpr*/ int sed_implied(cpu_registers& regs) noexcept {
  std::cout << "        ";
  regs.set_flag(cpu_flag::clear_decimal_mode);
  return 2;
}

[[nodiscard]] /*constexpr*/ int nop_implied() noexcept {
  std::cout << "        ";
  return 2;
}

[[nodiscard]] /*constexpr*/ int nop_immediate(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  // TODO: This is just here for the output log, can remove later as
  // immediate reads have no side effects
  mode::immediate_read(regs, mem);
  return 2;
}

[[nodiscard]] /*constexpr*/ int nop_zero_page(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  // TODO: This is just here for the output log, can remove later as
  // zero page reads have no side effects
  mode::zero_page(regs, mem);
  return 3;
}

[[nodiscard]] /*constexpr*/ int nop_zero_page_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  // TODO: This is just here for the output log, can remove later as
  // zero page reads have no side effects
  mode::zero_page_x(regs, mem);
  return 4;
}

[[nodiscard]] /*constexpr*/ int nop_absolute(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  // TODO: Absolute reads can have side effects?
  mem.read8(mode::absolute(regs, mem));
  return 4;
}

[[nodiscard]] /*constexpr*/ int nop_absolute_x(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto result = mode::absolute_indexed(regs, mem, regs.x());
  // TODO: Absolute reads can have side effects?
  mem.read8(result.address);
  return result.page_boundary_crossed ? 5 : 4;
}

[[nodiscard]] /*constexpr*/ int beq_relative(
    cpu_registers& regs,
    const ram_controller& mem) noexcept {
  auto relative_address = mode::relative(regs, mem);
  if (regs.flag(cpu_flag::zero)) {
    if (regs.offset_pc(relative_address)) {
      return 4;
    }
    return 3;
  }
  return 2;
}

[[nodiscard]] /*constexpr*/ int dcp_indirect_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto address = mode::indexed_indirect(regs, mem);
  dec(regs, mem, address);
  cmp(regs, regs.accumulator(), mem.read8(address));
  return 8;
}

[[nodiscard]] /*constexpr*/ int dcp_indirect_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto addressing = mode::indirect_indexed(regs, mem);
  dec(regs, mem, addressing.address);
  cmp(regs, regs.accumulator(), mem.read8(addressing.address));
  return 8;
}

[[nodiscard]] /*constexpr*/ int dcp_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  auto address = mode::zero_page(regs, mem);
  dec(regs, mem, address);
  cmp(regs, regs.accumulator(), mem.read8(address));
  return 5;
}

[[nodiscard]] /*constexpr*/ int dcp_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  auto address = mode::zero_page_x(regs, mem);
  dec(regs, mem, address);
  cmp(regs, regs.accumulator(), mem.read8(address));
  return 6;
}

[[nodiscard]] /*constexpr*/ int dcp_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  auto address = mode::absolute(regs, mem);
  dec(regs, mem, address);
  cmp(regs, regs.accumulator(), mem.read8(address));
  return 6;
}

[[nodiscard]] /*constexpr*/ int dcp_absolute_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto addressing = mode::absolute_indexed(regs, mem, regs.y());
  dec(regs, mem, addressing.address);
  cmp(regs, regs.accumulator(), mem.read8(addressing.address));
  return 7;
}

[[nodiscard]] /*constexpr*/ int dcp_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  auto addressing = mode::absolute_indexed(regs, mem, regs.x());
  dec(regs, mem, addressing.address);
  cmp(regs, regs.accumulator(), mem.read8(addressing.address));
  return 7;
}

/*constexpr*/ void isc(cpu_registers& regs,
                       ram_controller& mem,
                       std::uint16_t address) noexcept {
  inc(regs, mem, address);
  sbc(regs, mem.read8(address));
}

[[nodiscard]] /*constexpr*/ int isc_indirect_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  isc(regs, mem, mode::indexed_indirect(regs, mem));
  return 8;
}

[[nodiscard]] /*constexpr*/ int isc_indirect_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  isc(regs, mem, mode::indirect_indexed(regs, mem).address);
  return 8;
}

[[nodiscard]] /*constexpr*/ int isc_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  isc(regs, mem, mode::zero_page(regs, mem));
  return 5;
}

[[nodiscard]] /*constexpr*/ int isc_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  isc(regs, mem, mode::zero_page_x(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int isc_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  isc(regs, mem, mode::absolute(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int isc_absolute_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  isc(regs, mem, mode::absolute_indexed(regs, mem, regs.y()).address);
  return 7;
}

[[nodiscard]] /*constexpr*/ int isc_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  isc(regs, mem, mode::absolute_indexed(regs, mem, regs.x()).address);
  return 7;
}

/*constexpr*/ void slo(cpu_registers& regs,
                       ram_controller& mem,
                       std::uint16_t address) noexcept {
  asl(regs, mem, address);
  ora(regs, mem.read8(address));
}

[[nodiscard]] /*constexpr*/ int slo_indirect_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  slo(regs, mem, mode::indexed_indirect(regs, mem));
  return 8;
}

[[nodiscard]] /*constexpr*/ int slo_indirect_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  slo(regs, mem, mode::indirect_indexed(regs, mem).address);
  return 8;
}

[[nodiscard]] /*constexpr*/ int slo_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  slo(regs, mem, mode::zero_page(regs, mem));
  return 5;
}

[[nodiscard]] /*constexpr*/ int slo_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  slo(regs, mem, mode::zero_page_x(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int slo_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  slo(regs, mem, mode::absolute(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int slo_absolute_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  slo(regs, mem, mode::absolute_indexed(regs, mem, regs.y()).address);
  return 7;
}

[[nodiscard]] /*constexpr*/ int slo_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  slo(regs, mem, mode::absolute_indexed(regs, mem, regs.x()).address);
  return 7;
}

/*constexpr*/ void rla(cpu_registers& regs,
                       ram_controller& mem,
                       std::uint16_t address) noexcept {
  rol(regs, mem, address);
  and_instr(regs, mem, address);
}

[[nodiscard]] /*constexpr*/ int rla_indirect_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  rla(regs, mem, mode::indexed_indirect(regs, mem));
  return 8;
}

[[nodiscard]] /*constexpr*/ int rla_indirect_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  rla(regs, mem, mode::indirect_indexed(regs, mem).address);
  return 8;
}

[[nodiscard]] /*constexpr*/ int rla_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  rla(regs, mem, mode::zero_page(regs, mem));
  return 5;
}

[[nodiscard]] /*constexpr*/ int rla_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  rla(regs, mem, mode::zero_page_x(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int rla_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  rla(regs, mem, mode::absolute(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int rla_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  rla(regs, mem, mode::absolute_indexed(regs, mem, regs.x()).address);
  return 7;
}

[[nodiscard]] /*constexpr*/ int rla_absolute_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  rla(regs, mem, mode::absolute_indexed(regs, mem, regs.y()).address);
  return 7;
}

/*constexpr*/ void sre(cpu_registers& regs,
                       ram_controller& mem,
                       std::uint16_t address) noexcept {
  lsr(regs, mem, address);
  eor(regs, mem.read8(address));
}

[[nodiscard]] /*constexpr*/ int sre_indirect_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  sre(regs, mem, mode::indexed_indirect(regs, mem));
  return 8;
}

[[nodiscard]] /*constexpr*/ int sre_indirect_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  sre(regs, mem, mode::indirect_indexed(regs, mem).address);
  return 8;
}

[[nodiscard]] /*constexpr*/ int sre_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  sre(regs, mem, mode::zero_page(regs, mem));
  return 5;
}

[[nodiscard]] /*constexpr*/ int sre_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  sre(regs, mem, mode::zero_page_x(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int sre_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  sre(regs, mem, mode::absolute(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int sre_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  sre(regs, mem, mode::absolute_indexed(regs, mem, regs.x()).address);
  return 7;
}

[[nodiscard]] /*constexpr*/ int sre_absolute_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  sre(regs, mem, mode::absolute_indexed(regs, mem, regs.y()).address);
  return 7;
}

/*constexpr*/ void rra(cpu_registers& regs, ram_controller& mem, std::uint16_t address) noexcept {
  ror(regs, mem, address);
  adc(regs, mem.read8(address));
}

[[nodiscard]] /*constexpr*/ int rra_indirect_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  rra(regs, mem, mode::indexed_indirect(regs, mem));
  return 8;
}

[[nodiscard]] /*constexpr*/ int rra_indirect_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  rra(regs, mem, mode::indirect_indexed(regs, mem).address);
  return 8;
}

[[nodiscard]] /*constexpr*/ int rra_zero_page(cpu_registers& regs,
                                              ram_controller& mem) noexcept {
  rra(regs, mem, mode::zero_page(regs, mem));
  return 5;
}

[[nodiscard]] /*constexpr*/ int rra_zero_page_x(cpu_registers& regs,
                                                ram_controller& mem) noexcept {
  rra(regs, mem, mode::zero_page_x(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int rra_absolute(cpu_registers& regs,
                                             ram_controller& mem) noexcept {
  rra(regs, mem, mode::absolute(regs, mem));
  return 6;
}

[[nodiscard]] /*constexpr*/ int rra_absolute_x(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  rra(regs, mem, mode::absolute_indexed(regs, mem, regs.x()).address);
  return 7;
}

[[nodiscard]] /*constexpr*/ int rra_absolute_y(cpu_registers& regs,
                                               ram_controller& mem) noexcept {
  rra(regs, mem, mode::absolute_indexed(regs, mem, regs.y()).address);
  return 7;
}

}  // namespace opcode

#endif  // NES_OPCODES_H

#ifndef NES_CPU_H
#define NES_CPU_H

#include <cstdint>
#include <iostream>
#include "cpu_registers.h"
#include "opcodes.h"
#include "ram_controller.h"
#include "fmt/format.h"

class cpu2a03 {
 public:
  explicit cpu2a03(ram_controller& memory) : m_memory(memory) {}

  constexpr void reset() noexcept {
    m_registers = cpu_registers{};
    // m_registers.set_pc(m_memory.read16(0xFFFC));
    m_registers.set_pc(0xC000);
  }

  [[nodiscard]] /*constexpr*/ int process_instruction() noexcept {

    if(m_registers.pc() == 0xDC10) {
      int f = 4;
    }
    std::cout << fmt::format("{:04X}  ", m_registers.pc());
    auto opcode = m_memory.read8(m_registers.increment_pc());
    std::cout << fmt::format("{:02X} ", opcode);

    switch (opcode) {
      case 0x00:
        return opcode::brk_implied(m_registers, m_memory);
      case 0x01:
        return opcode::ora_indirect_x(m_registers, m_memory);
      case 0x03:
        return opcode::slo_indirect_x(m_registers, m_memory);
      case 0x04:
        return opcode::nop_zero_page(m_registers, m_memory);
      case 0x05:
        return opcode::ora_zero_page(m_registers, m_memory);
      case 0x06:
        return opcode::asl_zero_page(m_registers, m_memory);
      case 0x07:
        return opcode::slo_zero_page(m_registers, m_memory);
      case 0x08:
        return opcode::php_implied(m_registers, m_memory);
      case 0x09:
        return opcode::ora_immediate(m_registers, m_memory);
      case 0x0A:
        return opcode::asl_accumulator(m_registers);
      case 0x0C:
        return opcode::nop_absolute(m_registers, m_memory);
      case 0x0D:
        return opcode::ora_absolute(m_registers, m_memory);
      case 0x0E:
        return opcode::asl_absolute(m_registers, m_memory);
      case 0x0F:
        return opcode::slo_absolute(m_registers, m_memory);
      case 0x10:
        return opcode::bpl_relative(m_registers, m_memory);
      case 0x11:
        return opcode::ora_indirect_y(m_registers, m_memory);
      case 0x13:
        return opcode::slo_indirect_y(m_registers, m_memory);
      case 0x14:
        return opcode::nop_zero_page_x(m_registers, m_memory);
      case 0x15:
        return opcode::ora_zero_page_x(m_registers, m_memory);
      case 0x16:
        return opcode::asl_zero_page_x(m_registers, m_memory);
      case 0x17:
        return opcode::slo_zero_page_x(m_registers, m_memory);
      case 0x18:
        return opcode::clc_implied(m_registers);
      case 0x19:
        return opcode::ora_absolute_y(m_registers, m_memory);
      case 0x1A:
        return opcode::nop_implied();
      case 0x1B:
        return opcode::slo_absolute_y(m_registers, m_memory);
      case 0x1C:
        return opcode::nop_absolute_x(m_registers, m_memory);
      case 0x1D:
        return opcode::ora_absolute_x(m_registers, m_memory);
      case 0x1E:
        return opcode::asl_absolute_x(m_registers, m_memory);
      case 0x1F:
        return opcode::slo_absolute_x(m_registers, m_memory);
      case 0x20:
        return opcode::jsr_absolute(m_registers, m_memory);
      case 0x21:
        return opcode::and_indirect_x(m_registers, m_memory);
      case 0x23:
        return opcode::rla_indirect_x(m_registers, m_memory);
      case 0x24:
        return opcode::bit_zero_page(m_registers, m_memory);
      case 0x25:
        return opcode::and_zero_page(m_registers, m_memory);
      case 0x26:
        return opcode::rol_zero_page(m_registers, m_memory);
      case 0x27:
        return opcode::rla_zero_page(m_registers, m_memory);
      case 0x28:
        return opcode::plp_implied(m_registers, m_memory);
      case 0x29:
        return opcode::and_immediate(m_registers, m_memory);
      case 0x2A:
        return opcode::rol_accumulator(m_registers);
      case 0x2C:
        return opcode::bit_absolute(m_registers, m_memory);
      case 0x2D:
        return opcode::and_absolute(m_registers, m_memory);
      case 0x2E:
        return opcode::rol_absolute(m_registers, m_memory);
      case 0x2F:
        return opcode::rla_absolute(m_registers, m_memory);
      case 0x30:
        return opcode::bmi_relative(m_registers, m_memory);
      case 0x31:
        return opcode::and_indirect_y(m_registers, m_memory);
      case 0x33:
        return opcode::rla_indirect_y(m_registers, m_memory);
      case 0x34:
        return opcode::nop_zero_page_x(m_registers, m_memory);
      case 0x35:
        return opcode::and_zero_page_x(m_registers, m_memory);
      case 0x36:
        return opcode::rol_zero_page_x(m_registers, m_memory);
      case 0x37:
        return opcode::rla_zero_page_x(m_registers, m_memory);
      case 0x38:
        return opcode::sec_implied(m_registers);
      case 0x39:
        return opcode::and_absolute_y(m_registers, m_memory);
      case 0x3A:
        return opcode::nop_implied();
      case 0x3B:
        return opcode::rla_absolute_y(m_registers, m_memory);
      case 0x3C:
        return opcode::nop_absolute_x(m_registers, m_memory);
      case 0x3D:
        return opcode::and_absolute_x(m_registers, m_memory);
      case 0x3E:
        return opcode::rol_absolute_x(m_registers, m_memory);
      case 0x3F:
        return opcode::rla_absolute_x(m_registers, m_memory);
      case 0x40:
        return opcode::rti_implied(m_registers, m_memory);
      case 0x41:
        return opcode::eor_indirect_x(m_registers, m_memory);
      case 0x43:
        return opcode::sre_indirect_x(m_registers, m_memory);
      case 0x44:
        return opcode::nop_zero_page(m_registers, m_memory);
      case 0x45:
        return opcode::eor_zero_page(m_registers, m_memory);
      case 0x46:
        return opcode::lsr_zero_page(m_registers, m_memory);
      case 0x47:
        return opcode::sre_zero_page(m_registers, m_memory);
      case 0x48:
        return opcode::pha_implied(m_registers, m_memory);
      case 0x49:
        return opcode::eor_immediate(m_registers, m_memory);
      case 0x4A:
        return opcode::lsr_accumulator(m_registers);
      case 0x4C:
        return opcode::jmp_absolute(m_registers, m_memory);
      case 0x4D:
        return opcode::eor_absolute(m_registers, m_memory);
      case 0x4E:
        return opcode::lsr_absolute(m_registers, m_memory);
      case 0x4F:
        return opcode::sre_absolute(m_registers, m_memory);
      case 0x50:
        return opcode::bvc_relative(m_registers, m_memory);
      case 0x51:
        return opcode::eor_indirect_y(m_registers, m_memory);
      case 0x53:
        return opcode::sre_indirect_y(m_registers, m_memory);
      case 0x54:
        return opcode::nop_zero_page_x(m_registers, m_memory);
      case 0x55:
        return opcode::eor_zero_page_x(m_registers, m_memory);
      case 0x56:
        return opcode::lsr_zero_page_x(m_registers, m_memory);
      case 0x57:
        return opcode::sre_zero_page_x(m_registers, m_memory);
      case 0x58:
        return opcode::cli_implied(m_registers);
      case 0x59:
        return opcode::eor_absolute_y(m_registers, m_memory);
      case 0x5A:
        return opcode::nop_implied();
      case 0x5B:
        return opcode::sre_absolute_y(m_registers, m_memory);
      case 0x5C:
        return opcode::nop_absolute_x(m_registers, m_memory);
      case 0x5D:
        return opcode::eor_absolute_x(m_registers, m_memory);
      case 0x5E:
        return opcode::lsr_absolute_x(m_registers, m_memory);
      case 0x5F:
        return opcode::sre_absolute_x(m_registers, m_memory);
      case 0x60:
        return opcode::rts_implied(m_registers, m_memory);
      case 0x61:
        return opcode::adc_indirect_x(m_registers, m_memory);
      case 0x63:
        return opcode::rra_indirect_x(m_registers, m_memory);
      case 0x64:
        return opcode::nop_zero_page(m_registers, m_memory);
      case 0x65:
        return opcode::adc_zero_page(m_registers, m_memory);
      case 0x66:
        return opcode::ror_zero_page(m_registers, m_memory);
      case 0x67:
        return opcode::rra_zero_page(m_registers, m_memory);
      case 0x68:
        return opcode::pla_implied(m_registers, m_memory);
      case 0x69:
        return opcode::adc_immediate(m_registers, m_memory);
      case 0x6A:
        return opcode::ror_accumulator(m_registers);
      case 0x6C:
        return opcode::jmp_indirect(m_registers, m_memory);
      case 0x6D:
        return opcode::adc_absolute(m_registers, m_memory);
      case 0x6E:
        return opcode::ror_absolute(m_registers, m_memory);
      case 0x6F:
        return opcode::rra_absolute(m_registers, m_memory);
      case 0x70:
        return opcode::bvs_relative(m_registers, m_memory);
      case 0x71:
        return opcode::adc_indirect_y(m_registers, m_memory);
      case 0x73:
        return opcode::rra_indirect_y(m_registers, m_memory);
      case 0x74:
        return opcode::nop_zero_page_x(m_registers, m_memory);
      case 0x75:
        return opcode::adc_zero_page_x(m_registers, m_memory);
      case 0x76:
        return opcode::ror_zero_page_x(m_registers, m_memory);
      case 0x77:
        return opcode::rra_zero_page_x(m_registers, m_memory);
      case 0x78:
        return opcode::sei_implied(m_registers);
      case 0x79:
        return opcode::adc_absolute_y(m_registers, m_memory);
      case 0x7A:
        return opcode::nop_implied();
      case 0x7B:
        return opcode::rra_absolute_y(m_registers, m_memory);
      case 0x7C:
        return opcode::nop_absolute_x(m_registers, m_memory);
      case 0x7D:
        return opcode::adc_absolute_x(m_registers, m_memory);
      case 0x7E:
        return opcode::ror_absolute_x(m_registers, m_memory);
      case 0x7F:
        return opcode::rra_absolute_x(m_registers, m_memory);
      case 0x80:
        return opcode::nop_immediate(m_registers, m_memory);
      case 0x81:
        return opcode::sta_indirect_x(m_registers, m_memory);
      case 0x83:
        return opcode::sax_indirect_x(m_registers, m_memory);
      case 0x84:
        return opcode::sty_zero_page(m_registers, m_memory);
      case 0x85:
        return opcode::sta_zero_page(m_registers, m_memory);
      case 0x86:
        return opcode::stx_zero_page(m_registers, m_memory);
      case 0x87:
        return opcode::sax_zero_page(m_registers, m_memory);
      case 0x88:
        return opcode::dey_implied(m_registers);
      case 0x8A:
        return opcode::txa_implied(m_registers);
      case 0x8C:
        return opcode::sty_absolute(m_registers, m_memory);
      case 0x8D:
        return opcode::sta_absolute(m_registers, m_memory);
      case 0x8E:
        return opcode::stx_absolute(m_registers, m_memory);
      case 0x8F:
        return opcode::sax_absolute(m_registers, m_memory);
      case 0x90:
        return opcode::bcc_relative(m_registers, m_memory);
      case 0x91:
        return opcode::sta_indirect_y(m_registers, m_memory);
      case 0x94:
        return opcode::sty_zero_page_x(m_registers, m_memory);
      case 0x95:
        return opcode::sta_zero_page_x(m_registers, m_memory);
      case 0x96:
        return opcode::stx_zero_page_y(m_registers, m_memory);
      case 0x97:
        return opcode::sax_zero_page_y(m_registers, m_memory);
      case 0x98:
        return opcode::tya_implied(m_registers);
      case 0x99:
        return opcode::sta_absolute_y(m_registers, m_memory);
      case 0x9A:
        return opcode::txs_implied(m_registers);
      case 0x9D:
        return opcode::sta_absolute_x(m_registers, m_memory);
      case 0xA0:
        return opcode::ldy_immediate(m_registers, m_memory);
      case 0xA1:
        return opcode::lda_indirect_x(m_registers, m_memory);
      case 0xA2:
        return opcode::ldx_immediate(m_registers, m_memory);
      case 0xA3:
        return opcode::lax_indirect_x(m_registers, m_memory);
      case 0xA4:
        return opcode::ldy_zero_page(m_registers, m_memory);
      case 0xA5:
        return opcode::lda_zero_page(m_registers, m_memory);
      case 0xA6:
        return opcode::ldx_zero_page(m_registers, m_memory);
      case 0xA7:
        return opcode::lax_zero_page(m_registers, m_memory);
      case 0xA8:
        return opcode::tay_implied(m_registers);
      case 0xA9:
        return opcode::lda_immediate(m_registers, m_memory);
      case 0xAA:
        return opcode::tax_implied(m_registers);
      case 0xAC:
        return opcode::ldy_absolute(m_registers, m_memory);
      case 0xAD:
        return opcode::lda_absolute(m_registers, m_memory);
      case 0xAE:
        return opcode::ldx_absolute(m_registers, m_memory);
      case 0xAF:
        return opcode::lax_absolute(m_registers, m_memory);
      case 0xBA:
        return opcode::tsx_implied(m_registers);
      case 0xB0:
        return opcode::bcs_relative(m_registers, m_memory);
      case 0xB1:
        return opcode::lda_indirect_y(m_registers, m_memory);
      case 0xB3:
        return opcode::lax_indirect_y(m_registers, m_memory);
      case 0xB4:
        return opcode::ldy_zero_page_x(m_registers, m_memory);
      case 0xB5:
        return opcode::lda_zero_page_x(m_registers, m_memory);
      case 0xB6:
        return opcode::ldx_zero_page_y(m_registers, m_memory);
      case 0xB7:
        return opcode::lax_zero_page_y(m_registers, m_memory);
      case 0xB8:
        return opcode::clv_implied(m_registers);
      case 0xB9:
        return opcode::lda_absolute_y(m_registers, m_memory);
      case 0xBC:
        return opcode::ldy_absolute_x(m_registers, m_memory);
      case 0xBD:
        return opcode::lda_absolute_x(m_registers, m_memory);
      case 0xBE:
        return opcode::ldx_absolute_y(m_registers, m_memory);
      case 0xBF:
        return opcode::lax_absolute_y(m_registers, m_memory);
      case 0xC0:
        return opcode::cpy_immediate(m_registers, m_memory);
      case 0xC1:
        return opcode::cmp_indirect_x(m_registers, m_memory);
      case 0xC3:
        return opcode::dcp_indirect_x(m_registers, m_memory);
      case 0xC4:
        return opcode::cpy_zero_page(m_registers, m_memory);
      case 0xC5:
        return opcode::cmp_zero_page(m_registers, m_memory);
      case 0xC6:
        return opcode::dec_zero_page(m_registers, m_memory);
      case 0xC7:
        return opcode::dcp_zero_page(m_registers, m_memory);
      case 0xC8:
        return opcode::iny_implied(m_registers);
      case 0xC9:
        return opcode::cmp_immediate(m_registers, m_memory);
      case 0xCA:
        return opcode::dex_implied(m_registers);
      case 0xCC:
        return opcode::cpy_absolute(m_registers, m_memory);
      case 0xCD:
        return opcode::cmp_absolute(m_registers, m_memory);
      case 0xCE:
        return opcode::dec_absolute(m_registers, m_memory);
      case 0xCF:
        return opcode::dcp_absolute(m_registers, m_memory);
      case 0xD0:
        return opcode::bne_relative(m_registers, m_memory);
      case 0xD1:
        return opcode::cmp_indirect_y(m_registers, m_memory);
      case 0xD3:
        return opcode::dcp_indirect_y(m_registers, m_memory);
      case 0xD4:
        return opcode::nop_zero_page_x(m_registers, m_memory);
      case 0xD5:
        return opcode::cmp_zero_page_x(m_registers, m_memory);
      case 0xD6:
        return opcode::dec_zero_page_x(m_registers, m_memory);
      case 0xD7:
        return opcode::dcp_zero_page_x(m_registers, m_memory);
      case 0xD8:
        return opcode::cld_implied(m_registers);
      case 0xD9:
        return opcode::cmp_absolute_y(m_registers, m_memory);
      case 0xDA:
        return opcode::nop_implied();
      case 0xDB:
        return opcode::dcp_absolute_y(m_registers, m_memory);
      case 0xDC:
        return opcode::nop_absolute_x(m_registers, m_memory);
      case 0xDD:
        return opcode::cmp_absolute_x(m_registers, m_memory);
      case 0xDE:
        return opcode::dec_absolute_x(m_registers, m_memory);
      case 0xDF:
        return opcode::dcp_absolute_x(m_registers, m_memory);
      case 0xE0:
        return opcode::cpx_immediate(m_registers, m_memory);
      case 0xE1:
        return opcode::sbc_indirect_x(m_registers, m_memory);
      case 0xE3:
        return opcode::isc_indirect_x(m_registers, m_memory);
      case 0xE4:
        return opcode::cpx_zero_page(m_registers, m_memory);
      case 0xE5:
        return opcode::sbc_zero_page(m_registers, m_memory);
      case 0xE6:
        return opcode::inc_zero_page(m_registers, m_memory);
      case 0xE7:
        return opcode::isc_zero_page(m_registers, m_memory);
      case 0xE8:
        return opcode::inx_implied(m_registers);
      case 0xE9:
        return opcode::sbc_immediate(m_registers, m_memory);
      case 0xEA:
        return opcode::nop_implied();
      case 0xEB:
        return opcode::sbc_immediate(m_registers, m_memory);
      case 0xEC:
        return opcode::cpx_absolute(m_registers, m_memory);
      case 0xED:
        return opcode::sbc_absolute(m_registers, m_memory);
      case 0xEE:
        return opcode::inc_absolute(m_registers, m_memory);
      case 0xEF:
        return opcode::isc_absolute(m_registers, m_memory);
      case 0xF0:
        return opcode::beq_relative(m_registers, m_memory);
      case 0xF1:
        return opcode::sbc_indirect_y(m_registers, m_memory);
      case 0xF3:
        return opcode::isc_indirect_y(m_registers, m_memory);
      case 0xF4:
        return opcode::nop_zero_page_x(m_registers, m_memory);
      case 0xF5:
        return opcode::sbc_zero_page_x(m_registers, m_memory);
      case 0xF6:
        return opcode::inc_zero_page_x(m_registers, m_memory);
      case 0xF7:
        return opcode::isc_zero_page_x(m_registers, m_memory);
      case 0xF8:
        return opcode::sed_implied(m_registers);
      case 0xF9:
        return opcode::sbc_absolute_y(m_registers, m_memory);
      case 0xFA:
        return opcode::nop_implied();
      case 0xFB:
        return opcode::isc_absolute_y(m_registers, m_memory);
      case 0xFC:
        return opcode::nop_absolute_x(m_registers, m_memory);
      case 0xFD:
        return opcode::sbc_absolute_x(m_registers, m_memory);
      case 0xFE:
        return opcode::inc_absolute_x(m_registers, m_memory);
      case 0xFF:
        return opcode::isc_absolute_x(m_registers, m_memory);
      default:
        std::cerr << "Unhandled opcode " << std::hex << static_cast<int>(opcode)
                  << std::endl;
        abort();
    }
  }

  cpu_registers m_registers;

 private:
  ram_controller& m_memory;
};

#endif  // NES_CPU_H

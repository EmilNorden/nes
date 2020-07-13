#ifndef NES_CPU_REGISTERS_H
#define NES_CPU_REGISTERS_H

#include <cstdint>

enum class cpu_flag : std::uint8_t {
  carry = 0b00000001,
  zero = 0b00000010,
  interrupt_disable = 0b00000100,
  clear_decimal_mode = 0b00001000,
  break_command = 0b00010000,
  unused = 0b00100000,
  overflow = 0b01000000,
  sign = 0b10000000
};

class cpu_registers {
 public:
  cpu_registers()
      : m_accumulator(0),
        m_x(0),
        m_y(0),
        m_status(0b00100100),
        m_pc(0),
        m_stack(0x01FD) {}

  [[nodiscard]] constexpr auto accumulator() const noexcept {
    return m_accumulator;
  }
  [[nodiscard]] constexpr auto x() const noexcept { return m_x; }
  [[nodiscard]] constexpr auto y() const noexcept { return m_y; }
  [[nodiscard]] constexpr auto status() const noexcept { return m_status; }
  [[nodiscard]] constexpr auto stack() const noexcept { return m_stack; }
  [[nodiscard]] constexpr auto increment_pc() noexcept { return m_pc++; }
  [[nodiscard]] constexpr auto pc() noexcept { return m_pc; }

  constexpr void increment_stack() noexcept {
    if (++m_stack > 0x01FF) {
      m_stack = 0x0100;
    }
  }
  constexpr void decrement_stack() noexcept {
    if (m_stack == 0x0100) {
      m_stack = 0x01FF;
    } else {
      --m_stack;
    }
  }

  constexpr void set_accumulator(std::uint8_t value) noexcept {
    m_accumulator = value;
    set_flag_if(cpu_flag::zero, m_accumulator == 0);
    set_flag_if(cpu_flag::sign, (m_accumulator & 0b10000000U) == 0b10000000U);
  }

  constexpr void set_x(std::uint8_t value) noexcept {
    m_x = value;
    set_flag_if(cpu_flag::zero, m_x == 0);
    set_flag_if(cpu_flag::sign, (m_x & 0b10000000U) == 0b10000000U);
  }

  constexpr void set_y(std::uint8_t value) noexcept {
    m_y = value;
    set_flag_if(cpu_flag::zero, m_y == 0);
    set_flag_if(cpu_flag::sign, (m_y & 0b10000000U) == 0b10000000U);
  }

  constexpr void set_pc(std::uint16_t val) noexcept { m_pc = val; }

  [[nodiscard]] constexpr auto offset_pc(std::int8_t val) noexcept {
    // auto from_page = m_pc / 0x100;
    auto from_page = m_pc & 0xFF00;
    m_pc += val;
    // auto to_page = m_pc / 0x100;
    auto to_page = m_pc & 0xFF00;

    return from_page != to_page;
  }

  constexpr void set_stack(std::uint8_t val) noexcept {
    m_stack = 0x0100U | val;
  }

  constexpr void set_status(std::uint8_t val) noexcept {
    m_status = val;
  }

  [[nodiscard]] constexpr auto flag(cpu_flag f) const noexcept {
    return (m_status & static_cast<std::uint8_t>(f)) ==
           static_cast<std::uint8_t>(f);
  }

  constexpr void set_flag(cpu_flag f) noexcept {
    m_status = m_status | static_cast<std::uint8_t>(f);
  }

  constexpr void set_flag_if(cpu_flag flag, bool set) {
    if (set) {
      set_flag(flag);
    } else {
      clear_flag(flag);
    }
  }

  constexpr void clear_flag(cpu_flag f) noexcept {
    m_status =
        m_status & static_cast<std::uint8_t>(~static_cast<std::uint8_t>(f));
  }

 private:
  std::uint8_t m_accumulator;
  std::uint8_t m_x;
  std::uint8_t m_y;
  // 0 - (C) Carry flag
  // 1 - (Z) Zero flag
  // 2 - (I) Interrupt disable flag
  // 3 - (D) Decimal mode status flag
  // 4 - (B) Set when software interrupt (BRK instruction) is executed
  // 5 - Not used. Supposed to be logical 1 at all times
  // 6 - (V) Overflow flag
  // 7 - (S) Sign flag
  std::uint8_t m_status;

  std::uint16_t m_pc;
  std::uint16_t m_stack;
};

#endif  // NES_CPU_REGISTERS_H

#ifndef NES_COMMON_H
#define NES_COMMON_H

#include <cstdint>

template<typename Type>
class strong_typedef{
 public:
  constexpr explicit strong_typedef(const Type& value) : m_value(value) {}
  [[nodiscard]] constexpr explicit operator Type() const noexcept { return m_value; }
 private:
  Type m_value;
};

 class address : public strong_typedef<std::uint16_t>
 {
 };



#endif  // NES_COMMON_H

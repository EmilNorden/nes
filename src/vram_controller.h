#ifndef NES_VRAM_CONTROLLER_H
#define NES_VRAM_CONTROLLER_H

#include <cstdint>

class vram_controller {
 public:

 private:
  std::uint8_t m_memory[0x4000];
};

#endif  // NES_VRAM_CONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vk_sdk_platform.h>
#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include <vulkan/vulkan.hpp>
#include "cpu.h"
#include "fmt/format.h"
#include "ppu.h"
#include "ppu_registers.h"
#include "rom_loader.h"

int main() {
  if (SDL_Init(SDL_INIT_EVERYTHING) == 1) {
    std::cerr << "Unable to init SDL: " << SDL_GetError() << std::endl;
    return 1;
  }
  // auto window = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED,
  // SDL_WINDOWPOS_CENTERED, 512, 512, SDL_WINDOW_VULKAN);
  auto window = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, 512, 480, 0);

  if (!window) {
    std::cerr << "Unable to create window: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_Surface* window_surface = SDL_GetWindowSurface(window);
  if (!window_surface) {
    std::cerr << "Unable to get window surface: " << SDL_GetError()
              << std::endl;
    return 1;
  }

  /*unsigned int count;
  if(!SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr)) {
    std::cerr << "Unable get extensions: " << SDL_GetError() << std::endl;
  }*/

  /*std::vector<const char*> extensions;
  extensions.reserve(count);
  if(!SDL_Vulkan_GetInstanceExtensions(window, &count, extensions.data())) {
    std::cerr << "Unable get extensions again: " << SDL_GetError() << std::endl;
  }*/

  /*VkInstanceCreateInfo create_info = {};
  create_info.enabledExtensionCount =
  static_cast<std::uint32_t>(extensions.size());
  create_info.ppEnabledExtensionNames = extensions.data();*/

  /*VkInstance instance;
  auto result = vkCreateInstance(&create_info, nullptr, &instance);
  if(VK_SUCCESS != result) {
    std::cerr << "Cant create instance" << std::endl;
  }*/

  auto surface = SDL_CreateRGBSurface(0, 256, 240, 32, 0x000000ff, 0x00000ff00,
                                      0x00ff0000, 0xff000000);
  if (!surface) {
    std::cerr << "Unable to allocate surface: " << SDL_GetError() << std::endl;
    return 1;
  }
  // std::ifstream f{"../../roms/nestest.nes", std::ios::binary};
  std::ifstream f{"../../../donkey_kong.nes", std::ios::binary};

  auto a = load_rom(f);

  ppu_registers ppu_regs{};
  vram_controller vram{ppu_regs};
  ram_controller ram{ppu_regs, vram};
  cpu2a03 cpu{ram};
  ppu2c02 ppu{vram, ppu_regs};

  if (a.prg_rom().size() > 1) {
    ram.load_prg_bank1(a.prg_rom()[0]);
    ram.load_prg_bank2(a.prg_rom()[1]);
  } else {
    ram.load_prg_bank1(a.prg_rom()[0]);
    ram.load_prg_bank2(a.prg_rom()[0]);
  }

  cpu.reset();
  ram.write8(0x2002, 0x80);
  int iterations = 0;
  int total_cycles = 0;
  bool run = true;
  while (run) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          run = false;
      }
    }

    auto regs = cpu.m_registers;
    auto cpu_cycles = cpu.process_instruction();
    std::cout << fmt::format(
        "A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP: {:02X} CYC: {:>3}",
        regs.accumulator(), regs.x(), regs.y(), regs.status(),
        regs.stack() & 0xFFU, total_cycles);

    total_cycles += cpu_cycles * 3;
    std::cout << std::endl;

    SDL_LockSurface(surface);
    unsigned int* ptr = static_cast<unsigned int*>(surface->pixels);
    ppu.process(cpu_cycles * 3, ptr);
    SDL_UnlockSurface(surface);


    SDL_LockSurface(surface);

    for(int y = 0; y < 240; ++y) {
      for(int x = 0; x < 256; ++x) {
        // ptr[(y * 256) + x] = 0xFF0000FF;
      }
    }
    SDL_UnlockSurface(surface);

    SDL_BlitSurface(surface, nullptr, window_surface, nullptr);

    if (++iterations > 10000) {
      // break;
    }

    SDL_UpdateWindowSurface(window);
  }
}
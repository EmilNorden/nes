
set(CPP_SOURCES
		main.cpp
		cartridge.h
		cpu.h
		cpu_registers.h
		opcodes.h
		ppu.h
		prg_rom_bank.h
		ram_controller.h
		rom_loader.h
		vram_controller.h)

add_executable(nes ${CPP_SOURCES})

find_package(fmt REQUIRED)
set_target_properties(nes PROPERTIES CXX_STANDARD 17)

target_link_libraries(nes fmt::fmt)

target_compile_options(nes PRIVATE
		-march=haswell
		-Wall
		-Wextra
		-Wshadow
		-Wnon-virtual-dtor
		-Wold-style-cast
		-Wcast-align
		-Wunused
		-Woverloaded-virtual
		-Wpedantic -Wconversion
		-Wsign-conversion
		$<$<CXX_COMPILER_ID:GNU>: -Wmisleading-indentation>
		$<$<CXX_COMPILER_ID:GNU>: -Wduplicated-cond>
		$<$<CXX_COMPILER_ID:GNU>: -Wduplicated-branches>
		$<$<CXX_COMPILER_ID:GNU>: -Wlogical-op>
		$<$<CXX_COMPILER_ID:GNU>: -Wuseless-cast>
        $<$<CXX_COMPILER_ID:Clang>: -Wundefined-reinterpret-cast>
		-Wnull-dereference
		-Wdouble-promotion
		-Wformat=2)
# target_link_options(nes PUBLIC ...)

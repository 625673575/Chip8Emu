#include "memory.h"
#include<memory>

namespace memory {
	u8 RAM[4096];
	void InitializeFont()
	{
		std::memcpy(memory::RAM, fontSet, sizeof(fontSet));
	}
	void loadProgram(u8* program, size_t length)
	{
		std::memset(RAM, 0, sizeof(RAM));//zero memory
		InitializeFont();//copy font data to the 0x00-0x80 address 
		std::memcpy(RAM + PROGRAM_START_RAM_ADDR, program, length);
	}
	inline u8 Read(u16 addr) {
		if (addr >= FULL_RAM_SIZE)return 0x0;
		return RAM[addr];
	}
	inline void Write(u16 addr, u8 input)
	{
		RAM[addr] = input;
	}
}
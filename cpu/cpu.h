#ifndef _CHIP8_CPU_
#define _CHIP8_CPU_
#include <stack>
#include <random>
#include "../common/types.h"
namespace cpu
{
	static constexpr u16 OP_LEN = 2;
	extern u16 opcode;			//current opcode
	extern u8 r[16];			//register R[0-15]
	extern u16 pc;				//program counter;
	extern u16 I;				//index register
	extern u16 sp;				//stack pointer;
	extern u16 stack[16];
	extern u16 programAddress();
	extern void reset();
	extern void interrupt();
	extern void retrieve();
	extern bool isHalt();
	extern void loadProgram(u8* program, size_t length);
	extern void emulateCycle(uint32_t sleep = 0, bool forceEmu = false);
};
#endif


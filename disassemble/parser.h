#pragma once
#include "instruction.hpp"
namespace disasm {
	//parse machine code to asm instruction
	extern instruction parse(u16 val, u16 l=0);
}

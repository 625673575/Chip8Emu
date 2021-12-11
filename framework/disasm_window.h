#pragma once
#include "imgui_window.hpp"
#include "../disassemble/parser.h"
#include <vector>
using namespace disasm;
namespace framework {
	class disasm_window :public imgui_window<disasm_window>
	{
	public:
		disasm_window();
		void disassembly(u8*,long);
	protected:
		void OnGUI()override;

		u8* bin;
		long bin_size;
		std::vector<instruction> inst;
	};
}


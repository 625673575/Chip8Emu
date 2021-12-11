#include "disasm_window.h"
namespace framework {
	disasm_window::disasm_window() :bin(nullptr), bin_size(0)
	{
	}
	void disasm_window::OnGUI()
	{
		ImGui::Begin("ASM!");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		for (const auto& v : inst) {
			std::visit([](auto&& i) {
				ImGui::Text(i.get_asm().c_str());
				}, v);
		}

		ImGui::End();
	}
	void disasm_window::disassembly(u8* _data, long _size)
	{
		bin = _data;
		bin_size = _size;
		for (int i = 0; i < _size; i += 2) {
			u16 opcode = u16(bin[i]) << 8 | bin[i + 1];
			inst.push_back(parse(opcode, i));
		}
	}
}
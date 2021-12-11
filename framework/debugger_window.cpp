#include "debugger_window.h"
#include "../common/io.h"
namespace framework {

	void debugger_window::OnGUI()
	{
		showMenu();
		if (core != nullptr) {
			showDebugger();
			showRegister();
			if (bShowMemory && memory)
				showMemory();
		}
	}

	void debugger_window::OnEvent(const SDL_Event* e)
	{
		if (e->type == SDL_QUIT) {
			exit(0);
		}
		if (e->type == SDL_KEYDOWN) {
			if (e->key.keysym.mod & KMOD_CTRL) {
				switch (e->key.keysym.sym) {
				case  SDLK_F5:
					if (!core->is_run())
						core->run();
					break;
				case SDLK_o:
					openRom();
					break;
				}
			}
			if (e->key.keysym.mod & KMOD_SHIFT) {
				switch (e->key.keysym.sym) {
				case  SDLK_F5:
					if (core->is_run())
						core->stop();
					break;
				}
			}
			switch (e->key.keysym.sym) {
			case  SDLK_F5:
				if (core->is_run()) {
					if (core->is_hit_break())
						core->continue_from_break_point();
				}
				else {
					core->debug();
				}
				break;
			case  SDLK_F10:
				if (core->is_hit_break())
					core->continue_one_op();
				break;
			}
		}
	}

	void debugger_window::showMenu()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", "Ctrl+O")) {
					openRom();
				}
				if (ImGui::BeginMenu("Open Recent"))
				{
					ImGui::MenuItem("fish_hat.c");
					ImGui::MenuItem("fish_hat.inl");
					ImGui::MenuItem("fish_hat.h");
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (core) {
				if (ImGui::BeginMenu("Debug"))
				{
					if (ImGui::MenuItem("Debug", "F5")) {
						core->debug();
					}
					if (ImGui::MenuItem("Run", "Ctrl+F5")) {
						core->run();
					}
					if (ImGui::MenuItem("Stop", "Shift+F5")) {
						core->stop();
					}
					if (core->is_hit_break() && ImGui::MenuItem("Continue", "F5")) {
						core->continue_from_break_point();
					}
					if (core->is_hit_break() && ImGui::MenuItem("Continue", "F10")) {
						core->continue_one_op();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Tool"))
				{
					if (ImGui::MenuItem("Memory", "Ctrl+M")) {
						bShowMemory = !bShowMemory;
						memory = core->look_memory(0);
					}
					ImGui::EndMenu();
				}
			}
			ImGui::EndMainMenuBar();
		}
	}

	void debugger_window::showDebugger()
	{
		ImGui::Begin("ASM");
		static char DEBUG_STR[8] = " O ";
		int i = 0;
		for (const auto& v : core->get_asm()) {
			++i;
			ImGui::PushID(i);
			if (core->is_hit_line(v.location())) {
				ImGui::Button(v.is_break ? " 0 " : "   ");
				ImGui::SameLine();
				std::visit([](auto&& i) {
					ImGui::TextColored(ImVec4(1.0f, 0.25f, 0.25f, 1.0f), i.get_asm().c_str());
					}, v.asm_inst);
				ImGui::PopID();
				continue;
			}
			if (v.is_break) {
				if (ImGui::Button(" 0 ")) {
					v.is_break = false;
				}
				ImGui::SameLine();
				if (cpu::programAddress() == v.location() && core->hit_break_point()) {
					std::visit([](auto&& i) {
						ImGui::TextColored(ImVec4(1.0f, 0.25f, 0.25f, 1.0f), i.get_asm().c_str());
						}, v.asm_inst);
				}
				else {
					std::visit([](auto&& i) {
						ImGui::Text(i.get_asm().c_str());
						}, v.asm_inst);
				}
			}
			else {
				if (ImGui::Button("  ")) {
					v.is_break = true;
				}
				ImGui::SameLine();
				std::visit([](auto&& i) {
					ImGui::Text(i.get_asm().c_str());
					}, v.asm_inst);
			}
			ImGui::PopID();
		}

		ImGui::End();
	}

	void debugger_window::showRegister()
	{
#define SHOW_REG(reg,text)char reg##Buffer[16] = "0x";\
		char* reg##Start = &reg##Buffer[2];\
		itoa(cpu::##reg,reg##Start, 16);\
		if (ImGui::InputText(#text, reg##Start, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))\
		{\
			if (strlen(reg##Start) > 0)cpu::##reg = string_util::hex2int(reg##Buffer);\
			else cpu::##reg = 0;\
		}

		char intBuffer[16];
		if (ImGui::Begin("Register")) {
			SHOW_REG(pc, "pc")SHOW_REG(I, "I")SHOW_REG(sp, "sp")

				if (ImGui::CollapsingHeader("stack")) {
					char stack[16];
					for (int i = 0; i < ARRAYSIZE(cpu::stack); i++) {
						sprintf(stack, "STACK[%x]: ", i);
						itoa(cpu::stack[i], intBuffer, 16);
						ImGui::LabelText(stack, intBuffer);
					}
				}

			if (ImGui::CollapsingHeader("R")) {
				char r[8];
				for (int i = 0; i < ARRAYSIZE(cpu::r); i++) {
					sprintf(r, "r[%x]: ", i);
					itoa(cpu::r[i], intBuffer, 16);
					ImGui::LabelText(r, intBuffer);
				}
			}
			ImGui::End();
		}
	}

	void debugger_window::showMemory()
	{
		mem_edit.DrawWindow("Memory Editor", memory, FULL_RAM_SIZE);
	}

	void debugger_window::openRom()
	{
		std::string Filename;
		memory = nullptr;
		if (io::openFileDialog({ {"*"} }, Filename)) {
			log::Debug("open file %s,", Filename.c_str());
			core = std::make_unique<debugger>(Filename.c_str());
		}
	}

}
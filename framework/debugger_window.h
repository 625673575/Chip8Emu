#pragma once
#include "imgui_window.hpp"
#include "../debugger/debugger.h"
namespace framework {
	class debugger_window :public imgui_window<debugger_window>
	{
	protected:
		void OnGUI()override;
		void OnEvent(const SDL_Event* e) override;
	private:
		std::unique_ptr<debugger> core;
		void showMenu();
		void showDebugger();
		void showRegister();
		void showMemory();
		void openRom();
		bool bShowMemory;
		MemoryEditor mem_edit;
		u8* memory;
	};
}
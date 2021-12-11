#include "debugger.h"
#include "../memory/memory.h"

u8* debugger::look_memory(u16 addr) const
{
	if (addr < 0 || addr >= ARRAYSIZE(memory::RAM))
		return nullptr;
	return &memory::RAM[addr];
}

void debugger::set_asm(u16 addr, std::string opcode)
{
	if (opcode.length() != 4)
		printf("Opcode size is not 2 bytes");
	u16 op = std::stoi(opcode.substr(0, 2), nullptr, 16) << 8 | std::stoi(opcode.substr(2, 2), nullptr, 16);
	int expect_size = addr / 2 + 1;
	if (debug_asm.size() < expect_size) {
		int addSize = expect_size - debug_asm.size();
		for (int i = 0; i < addSize; i++)
			debug_asm.push_back({ parse(0, debug_asm.size() * 2),false });
	}
	debug_asm[addr / 2] = { parse(0, debug_asm.size() * 2),false };
}

void debugger::continue_one_op()
{
	if (!is_hit_break())return;
	std::lock_guard<std::mutex> cpu_guard(guard);
	is_debug_line = true;
	one_cycle(true);
}

void debugger::load(const char* path)
{
	//load program
	FILE* file = fopen(path, "rb");
	if (file == NULL) {
		fprintf(stderr, "WHERES THE FILE?!");
		exit(1);
	}
	fseek(file, 0l, SEEK_END);
	bin_size = ftell(file);
	fseek(file, 0l, SEEK_SET);
	auto ptr = (u8*)malloc(bin_size);
	bin.reset(ptr);
	fread(bin.get(), bin_size, 1, file);
	fclose(file);

	cpu::loadProgram(bin.get(), bin_size);
	disassembly();
}

debugger::debugger(const char* program_path) :running(false), is_debug_mode(false), is_debug_line(false)
{
	load(program_path);

	//init devices
	display::init();
	char title[] = "Chip8";
	display = std::make_unique<display::Display>();
	display->videoInit(title, SCREEN_WIDTH, SCREEN_HEIGHT, 10, true);
}

debugger::~debugger()
{
	if (running) stop();
}

void debugger::start_program()
{
	if (running) stop();
	running = true;
	display->setWindowVisible(true);
	cycle_thread = std::thread(&debugger::cycle, this);
	cycle_thread.detach();
}

void debugger::one_cycle(bool forceRun, uint32_t sleep)
{
	cpu::emulateCycle(sleep, forceRun);
	if (display != nullptr && display::drawFlag) {
		display->drawVideo(display::frameBuffer);
		display::drawFlag = false;
	}
}

void debugger::debug()
{
	is_debug_mode = true;
	start_program();
}

void debugger::run()
{
	is_debug_mode = false;
	start_program();
}

void debugger::stop()
{
	std::lock_guard<std::mutex> cpu_guard(guard);
	running = false;
	is_debug_mode = false;
	is_debug_line = false;
	cpu::reset();
	display::init();
	display->setWindowVisible(false);
}

void debugger::cycle()
{
	while (running) {
		guard.lock();
		one_cycle(false);
		guard.unlock();
		if (is_debug_mode && !is_debug_line) {
			if (hit_break_point()) {
				cpu::interrupt();
			}
			else {
				cpu::retrieve();
			}
		}
	}
}

void debugger::disassembly()
{
	for (long i = 0; i < bin_size; i += 2) {
		u16 opcode = u16(bin[i] << 8 | bin[i + 1]);
		debug_asm.push_back({ parse(opcode, i),false });
	}
}

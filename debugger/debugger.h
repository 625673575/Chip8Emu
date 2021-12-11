#pragma once
#include "../cpu/cpu.h"
#include "../display/display.h"
#include "../disassemble/parser.h"
#include <memory>
#include <atomic>
#include <thread>
#include <mutex>

using namespace disasm;
struct debug_instruction {
	instruction asm_inst;
	mutable bool is_break;
	u16 location() const {
		u16 r;
		std::visit([&r](auto&& i) {
			r = i.loc;
			}, asm_inst);
		return r;
	}

	u16 opcode() const {
		u16 r;
		std::visit([&r](auto&& i) {
			r = i.bin;
			}, asm_inst);
		return r;
	}
};
class debugger
{
private:
	std::unique_ptr<u8[]> bin;
	long bin_size;
	std::mutex guard;
	std::atomic_bool running;
	std::unique_ptr<display::Display> display;
	std::thread cycle_thread;
	std::vector<debug_instruction> debug_asm;
	bool is_debug_mode;
	// «∑Ò «÷”Ôæ‰÷¥––
	bool is_debug_line;
	void start_program();
	void one_cycle(bool forceRun, uint32_t sleep = 0);
public:
	void load(const char* path);
	void debug();
	void run();
	void stop();
	void cycle();
	void disassembly();
public:
	debugger(const char* program_path);
	~debugger();
	const std::vector<debug_instruction>& get_asm() const { return debug_asm; }
	debug_instruction& get_asm(u16 addr) { return debug_asm[addr / 2]; }
	u8* look_memory(u16 addr)const;
	void set_asm(u16 addr, std::string opcode);
	bool has_break_point(u16 addr) { return get_asm(addr).is_break; }
	bool hit_break_point() { return has_break_point(cpu::programAddress()); }
	void set_break_point(u16 addr) { auto& v = get_asm(addr); v.is_break = !v.is_break; }
	void set_break_point(u16 addr, bool force) { get_asm(addr).is_break = force; }
	void continue_from_break_point() {
		is_debug_line = false;
		cpu::retrieve();
	}
	void continue_one_op();
	bool is_hit_break() { return cpu::isHalt(); }
	bool is_run() { return running; }
	bool is_debug() { return is_run() && is_debug_mode; }
	bool is_hit_line(u16 addr) { return is_debug() && cpu::isHalt() && is_debug_line && addr == cpu ::programAddress(); }
};


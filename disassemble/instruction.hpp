#pragma once
#include "../common/types.h"
#include "../third-party/fmt/format.h"

#include <variant>
#include <sstream>
#include <iomanip>

namespace disasm {
	//多态的复制构造体（Polymorphic copy construction）
	//当我们用到多态时，经常会需要通过基类的指针来复制子对象。
	//通常我们可以通过在基类里面构造一个clone()虚函数，然后在每个子类里面定义这个clone()函数。使用CRTP可以让我们避免反复地在子类中去定义clone()函数或者constexpr的成员变量
	template <typename T>
	struct crtp {
		T& underlying() {
			return static_cast<T&>(*this);
		}
		[[nodiscard]] T const& underlying() const {
			return static_cast<T const&>(*this);
		}

	};

	template <typename T>
	struct instruction_base : crtp<T> {
		u16 bin;
		u16 loc;
		instruction_base() :bin(0x0), loc(0) {}
		instruction_base(u16 opcode, u16 location) :bin(opcode), loc(location) {}

		[[nodiscard]] u16 nnn() const {
			return bin & 0x0FFFu;
		}

		[[nodiscard]] u8 x() const {
			return (bin & 0x0F00u) >> 8u;
		}

		[[nodiscard]] u8 y() const {
			return (bin & 0x00F0u) >> 4u;
		}

		[[nodiscard]] u8 kk() const {
			return bin & 0x00FFu;
		}

		[[nodiscard]] u8 n() const {
			return bin & 0x000Fu;
		}

		[[nodiscard]] const char* get_mnemonic() const {
			return this->underlying().mnemonic;
		}

		[[nodiscard]] std::string get_c_equiv() const {
			auto nnn_s = fmt::format("{:#06x}", nnn());
			auto x_s = fmt::format("{:02x}", x());
			auto y_s = fmt::format("{:02x}", y());
			auto kk_s = fmt::format("{:02x}", kk());
			auto n_s = fmt::format("{:02x}", n());
			return fmt::format(this->underlying().cequiv,
				fmt::arg("NNNN", bin),
				fmt::arg("NNN", nnn()),
				fmt::arg("X", x()),
				fmt::arg("Y", y()),
				fmt::arg("NN", kk()),
				fmt::arg("N", n())
			);
		}
		[[nodiscard]] std::string get_asm() const {
			std::stringstream l;
			l << std::hex << std::uppercase << std::setw(8) << std::left << std::setfill(' ') << loc;
			std::stringstream op;
			op << std::hex << std::uppercase << std::setw(4) << std::left << std::setfill('0') << bin << "\t";
			std::stringstream mnemonic;
			mnemonic << std::setw(10) << std::left << std::setfill(' ') << get_mnemonic();
			std::stringstream equiv;
			equiv << std::setw(10) << std::left << std::setfill(' ') << get_c_equiv();
			std::stringstream ss;
			ss << l.str() << ":" << op.str() << mnemonic.str() << equiv.str();
			return ss.str();
		}
	};
	//0NNN 	Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs.
	struct SYS : public instruction_base<SYS> {
		constexpr static const char* mnemonic{ "SYS" };
		//constexpr static const char* description{"Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs."};
		constexpr static const char* cequiv{ "syscall()" };
	};
	//00E0 	Clears the screen.
	struct CLS : public instruction_base<CLS> {
		constexpr static const char* mnemonic{ "CLS" };
		constexpr static const char* cequiv{ "clear_screen()" };
	};
	//00EE 	Returns from a subroutine.
	struct RET : public instruction_base<RET> {
		constexpr static const char* mnemonic{ "RET" };
		constexpr static const char* cequiv{ "return" };
	};
	//1NNN 	Jumps to address NNN.
	struct JUMPI : public instruction_base<JUMPI> {
		constexpr static const char* mnemonic{ "JUMPI" };
		constexpr static const char* cequiv{ "goto {NNN:#05x}" };
	};
	//2NNN 	Calls subroutine at NNN.
	struct CALLI : public instruction_base<CALLI> {
		constexpr static const char* mnemonic{ "CALLI" };
		constexpr static const char* cequiv{ "call {NNN:#05x}" };
	};
	//3XNN 	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
	struct SKEI : public instruction_base<SKEI> {
		constexpr static const char* mnemonic{ "SKEI" };
		constexpr static const char* cequiv{ "if (V{X:x} == {NN:#04x})" };
	};
	//4XNN 	Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
	struct SKNEI : public instruction_base<SKNEI> {
		constexpr static const char* mnemonic{ "SKNEI" };
		constexpr static const char* cequiv{ "if (V{Y:x} != {NN:#04x})" };
	};
	//5XY0 	Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
	struct SKER : public instruction_base<SKER> {
		constexpr static const char* mnemonic{ "SKER" };
		constexpr static const char* cequiv{ "if (V{X:x} == V{Y:x})" };
	};
	//6XNN 	Sets VX to kk.
	struct SETI : public instruction_base<SETI> {
		constexpr static const char* mnemonic{ "SETI" };
		constexpr static const char* cequiv{ "V{X:1x} = {NN:#04x}" };
	};
	//7XNN 	Adds NN to VX. (Carry flag is not changed)
	struct ADDI : public instruction_base<ADDI> {
		constexpr static const char* mnemonic{ "ADDI" };
		constexpr static const char* cequiv{ "V{X:x} += {NN:#04x}" };
	};
	//8XY0 	Sets VX to the value of VY.
	struct SETR : public instruction_base<SETR> {
		constexpr static const char* mnemonic{ "SETR" };
		constexpr static const char* cequiv{ "V{X:x} = V{Y:x}" };
	};
	//8XY1 	Vx=Vx|Vy 	Sets VX to VX or VY. (Bitwise OR operation)
	struct ORR : public instruction_base<ORR> {
		constexpr static const char* mnemonic{ "ORR" };
		constexpr static const char* cequiv{ "V{X:x} |= V{Y:x}" };
	};
	//8XY2 	Vx=Vx&Vy 	Sets VX to VX and VY. (Bitwise AND operation)
	struct ANDR : public instruction_base<ANDR> {
		constexpr static const char* mnemonic{ "ANDR" };
		constexpr static const char* cequiv{ "V{X:x} &= V{Y:x}" };
	};
	//8XY3[a] 	BitOp 	Vx=Vx^Vy 	Sets VX to VX xor VY.
	struct XORR : public instruction_base<XORR> {
		constexpr static const char* mnemonic{ "XORR" };
		constexpr static const char* cequiv{ "V{X:x} ^= V{Y:x}" };
	};
	//8XY4 	Math 	Vx += Vy 	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
	struct ADDR : public instruction_base<ADDR> {
		constexpr static const char* mnemonic{ "ADDR" };
		constexpr static const char* cequiv{ "V{X:x} += V{Y:x}" };
	};
	//8XY5 	Math 	Vx -= Vy 	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
	struct SUBR : public instruction_base<SUBR> {
		constexpr static const char* mnemonic{ "SUBR" };
		constexpr static const char* cequiv{ "V{X:x} -= V{Y:x}" };
	};
	//8XY6[a] 	BitOp 	Vx>>=1 	Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[b]
	struct SHR : public instruction_base<SHR> {
		constexpr static const char* mnemonic{ "SHR" };
		constexpr static const char* cequiv{ "V{X:x} >>= 1" };
	};
	//8XY7[a] 	Math 	Vx=Vy-Vx 	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
	struct SUBR_R : public instruction_base<SUBR_R> {
		constexpr static const char* mnemonic{ "SUBR_R" };
		constexpr static const char* cequiv{ "V{X:x} = V{Y:x} - V{X:x}" };
	};
	//8XYE[a] 	BitOp 	Vx<<=1 	Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[b]
	struct SHL : public instruction_base<SHL> {
		constexpr static const char* mnemonic{ "SHL" };
		constexpr static const char* cequiv{ "V{X:x} <<= 1" };
	};
	//9XY0 	Cond 	if(Vx!=Vy) 	Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
	struct SKNER : public instruction_base<SKNER> {
		constexpr static const char* mnemonic{ "SKNER" };
		constexpr static const char* cequiv{ "if (V{X:x} != V{Y:x})" };
	};
	//ANNN 	MEM 	I = NNN 	Sets I to the address NNN.
	struct ISETI : public instruction_base<ISETI> {
		constexpr static const char* mnemonic{ "ISETI" };
		constexpr static const char* cequiv{ "I = {NNN:#05x}" };
	};
	//BNNN 	Flow 	PC=V0+NNN 	Jumps to the address NNN plus V0.
	struct JUMPIND : public instruction_base<JUMPIND> {
		constexpr static const char* mnemonic{ "JUMPIND" };
		constexpr static const char* cequiv{ "PC = V{X:x} + {NNN:#05x}" };
	};
	//CXNN 	Rand 	Vx=rand()&NN 	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
	struct RAND : public instruction_base<RAND> {
		constexpr static const char* mnemonic{ "RAND" };
		constexpr static const char* cequiv{ "V{X:x} = rand() & {NN:#04x}" };
	};
	//DXYN 	Disp 	draw(Vx,Vy,N) 	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
	struct DRAW : public instruction_base<DRAW> {
		constexpr static const char* mnemonic{ "DRAW" };
		constexpr static const char* cequiv{ "draw(V{X:x}, V{Y:x}, {N})" };
	};
	//EX9E 	KeyOp 	if(key()==Vx) 	Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
	struct SKEK : public instruction_base<SKEK> {
		constexpr static const char* mnemonic{ "SKEK" };
		constexpr static const char* cequiv{ "if (key() == V{X:x})" };
	};
	//EXA1 	KeyOp 	if(key()!=Vx) 	Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
	struct SKNEK : public instruction_base<SKNEK> {
		constexpr static const char* mnemonic{ "SKNEK" };
		constexpr static const char* cequiv{ "if (key() != V{X:x})" };
	};
	//FX07 	Timer 	Vx = get_delay() 	Sets VX to the value of the delay timer.
	struct GETD : public instruction_base<GETD> {
		constexpr static const char* mnemonic{ "GETD" };
		constexpr static const char* cequiv{ "V{X:x} = get_delay()" };
	};
	//FX0A 	KeyOp 	Vx = get_key() 	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
	struct KEY : public instruction_base<KEY> {
		constexpr static const char* mnemonic{ "KEY" };
		constexpr static const char* cequiv{ "V{X:x} = get_key()" };
	};
	//FX15 	Timer 	delay_timer(Vx) 	Sets the delay timer to VX.
	struct SETD : public instruction_base<SETD> {
		constexpr static const char* mnemonic{ "SETD" };
		constexpr static const char* cequiv{ "delay = V{X:x}" };
	};
	//FX18 	Sound 	sound_timer(Vx) 	Sets the sound timer to VX.
	struct SETS : public instruction_base<SETS> {
		constexpr static const char* mnemonic{ "SETS" };
		constexpr static const char* cequiv{ "sound = V{X:x}" };
	};
	//FX1E 	MEM 	I +=Vx 	Adds VX to I. VF is not affected.[c]
	struct IADDR : public instruction_base<IADDR> {
		constexpr static const char* mnemonic{ "IADDR" };
		constexpr static const char* cequiv{ "I += V{X:x}" };
	};
	//FX29 	MEM 	I=sprite_addr[Vx] 	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
	struct SPRITE : public instruction_base<SPRITE> {
		constexpr static const char* mnemonic{ "SPRITE" };
		constexpr static const char* cequiv{ "I = sprite(V{X:x})" };
	};

	struct UNKNOWN : public instruction_base<UNKNOWN> {
		constexpr static const char* mnemonic{ "UNKNOWN" };
		constexpr static const char* cequiv{ "I = UNKNOWN {NNNN:#06x}" };
	};
	//FX33 	BCD 	set_BCD(Vx);
	//
	//*(I+0)=BCD(3);
	//
	//*(I+1)=BCD(2);
	//
	//*(I+2)=BCD(1);
	//	Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
	struct BCD : public instruction_base<BCD> {
		constexpr static const char* mnemonic{ "BCD" };
		constexpr static const char* cequiv{ "BCD()" };
	};
	//FX55 	MEM 	reg_dump(Vx,&I) 	Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d]
	struct STORE : public instruction_base<STORE> {
		constexpr static const char* mnemonic{ "STORE" };
		constexpr static const char* cequiv{ "store()" };
	};
	//FX65 	MEM 	reg_load(Vx,&I) 	Fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d]
	struct LOAD : public instruction_base<LOAD> {
		constexpr static const char* mnemonic{ "LOAD" };
		constexpr static const char* cequiv{ "load()" };
	};
	using instruction = std::variant<SYS, CLS, RET, JUMPI, CALLI, SKEI, SKNEI, SKER, ADDI, SETI, SETR, ORR, ANDR, XORR, ADDR, SUBR, SUBR_R,
		SHR, SHL, SKNER, ISETI, JUMPIND, RAND, DRAW, SKEK, SKNEK, GETD, KEY, SETD, SETS, IADDR, SPRITE, BCD, STORE, LOAD, UNKNOWN>;

	template<typename T>
	bool is(const instruction& i) {
		return std::holds_alternative<T>(i);
	}
}
#include "cpu.h"
#include <ctime>
#include <cstdio>
#include <mutex>
#include "../memory/memory.h"
#include "../display/display.h"
#include "../input/keyboard.h"
namespace cpu
{
	u16 opcode;			//current opcode
	u8 r[16];			//register R[0-15]
	u16 pc;				//program counter;
	u16 I;				//index register
	u16 sp;				//stack pointer;
	u16 stack[16];
	u8 delay_timer;
	u8 sound_timer;

	bool halt;
	void interrupt() { halt = true; }
	void retrieve() { halt = false; }
	bool isHalt() { return halt; }
	void reset() {
		pc = 0x200;    // The first 512(0x200) bytes of memory::RAM are reserved
		opcode = 0;        // Reset the current opcode
		I = 0;        // Reset index register
		sp = 0;        // Reset stack pointer

		// Clear the stack
		for (int i = 0; i < 16; ++i)
			stack[i] = 0;

		// Clear the registers
		memset(r, 0, 16);

		// Reset the timers
		delay_timer = 0;
		sound_timer = 0;

		halt = false;

		srand(time(NULL));
	}
	void loadProgram(u8* program, size_t length)
	{
		memory::loadProgram(program, length);

		reset();
	}
	inline u16 programAddress() {
		return pc - 0x200;
	}
	void emulateCycle(uint32_t sleep,bool forceEmu)
	{
		if (!forceEmu && halt)return;
		if (sleep > 0)
			Sleep(sleep);
		opcode = memory::RAM[pc] << 8 | memory::RAM[pc + 1]; //from pc get 2 bytes as current opcode
		u8 n = opcode & 0x000F;    //lowest 4 bits 000N
		u8 kk = opcode & 0x00FF;   //lowest 8 bits 00NN
		u16 nnn = opcode & 0x0FFF; //lowest 12 bits 0NNN
		uint8_t x = opcode >> 8 & 0x000F; // lower 4 bits of the high byte 0X00
		uint8_t y = opcode >> 4 & 0x000F; // upper 4 bits of the low byte  00Y0

		switch (opcode & 0xF000) {
		case 0x0000:
			switch (n) {
			case 0x0:						//0x00E0 Clears the screen.
				log::Debug("CLS\n");
				display::init();
				display::drawFlag = true;
				pc += OP_LEN;
				break;

			case 0xE:						//0x00EE return from a subroutine
				log::Debug("RET\n");
				--sp;						//pop the stack, -- stack pointer 
				pc = stack[sp];				//set pc to last stack pointer
				pc += OP_LEN;					//pc+=2 to continue excution
				break;

			default:
				log::Debug("Unknown opcode [0x0000]: 0x%X\n", opcode);
			}
			break;

		case 0x1000:						// Jump to location nnn
			log::Debug("JP %X\n", nnn);
			pc = nnn;
			break;

		case 0x2000:						//call subroutine at nnn
			log::Debug("CALL %X\n", nnn);
			stack[sp] = pc;					//first,store the pc to stack 
			++sp;							//add sp
			pc = nnn;						//jump to nnn to contine excution
			break;

		case 0x3000:						//skips the next instruction if r[X] equals kk.
			log::Debug("SE V%X, %X\n", x, kk);
			if (r[x] == kk)
				pc += OP_LEN * 2;
			else
				pc += OP_LEN;
			break;

		case 0x4000:						//skips the next instruction if r[X] not equals kk.
			log::Debug("SNE V%X, %X\n", x, kk);
			if (r[x] != kk)
				pc += OP_LEN * 2;
			else
				pc += OP_LEN;
			break;

		case 0x5000:						//skips the next instruction if r[X] equals r[Y].
			log::Debug("SE V%X, V%X\n", x, y);
			if (r[x] == r[y])
				pc += OP_LEN * 2;
			else
				pc += OP_LEN;
			break;

		case 0x6000:						// Set r[x] =  kk
			log::Debug("LD V%X, %X\n", x, kk);
			r[x] = kk;
			pc += OP_LEN;
			break;

		case 0x7000:						// Set r[x] = r[x] + kk
			log::Debug("LD V%X, %X\n", x, kk);
			r[x] += kk;
			pc += 2;
			break;

		case 0x8000:
			switch (n) {
			case 0x0000:
				// Set r[x] = r[y]
				log::Debug(" LD V%X, V%X\n", x, y);
				r[x] = r[y];
				pc += 2;
				break;

			case 0x0001:
				// Set r[x] = r[x] OR r[y]
				log::Debug("OR V%X, V%X\n", x, y);
				r[x] |= r[y];
				pc += 2;
				break;

			case 0x0002:
				// Set r[x] = r[x] AND r[y]
				log::Debug("AND V%X, V%X\n", x, y);
				r[x] &= r[y];
				pc += 2;
				break;

			case 0x0003:
				// Set r[x] = r[x] XOR r[y]
				log::Debug("XOR V%X, V%X\n", x, y);
				r[x] ^= r[y];
				pc += 2;
				break;

			case 0x0004:
				// Set r[x] = r[x] + r[y], set VF = carry
				log::Debug("ADD V%X, V%X\n", x, y);
				r[0xF] = (r[y] > (0xFF - r[x])) ? 1 : 0;
				r[x] += r[y];
				pc += 2;
				break;

			case 0x0005:
				// Set r[x] = r[x] - r[y], set VF = NOT borrow
				log::Debug("SUB V%X, V%X\n", x, y);
				r[0xF] = (r[y] > r[x]) ? 0 : 1;
				r[x] -= r[y];
				pc += 2;
				break;

			case 0x0006:
				// Shifts r[x] right by one.
				// VF is set to the value of the LSB of r[x] before the shift
				log::Debug("SHR V%X\n", x);
				r[0xF] = r[x] & 0x1;
				r[x] >>= 1;
				pc += 2;
				break;

			case 0x0007:				// Set r[x] = r[y] - r[x], set VF = NOT borrow
				log::Debug("SUBN V%X, V%X\n", x, y);
				r[0xF] = (r[x] > r[y]) ? 0 : 1;
				r[x] = r[y] - r[x];
				pc += 2;
				break;

			case 0x000E:								// Shifts r[x] left by one.
														// r[0xF] is set to the value of the MSB of r[x] before the shift
				log::Debug("SHL V%X\n", x);
				r[0xF] = r[x] >> 7;
				r[x] <<= 1;
				pc += 2;
				break;

			default:
				log::Debug("Unknown opcode [0x8000]: 0x%X\n", opcode);
			}
			break;

		case 0x9000:								// Skip next instruction if r[x] != r[y]
			log::Debug("SNE V%X, V%X\n", x, y);
			if (r[x] != r[y])
				pc += 4;
			else
				pc += 2;
			break;

		case 0xA000:								// Set I = nnn
			log::Debug("LD I, %X\n", nnn);
			I = nnn;
			pc += 2;
			break;
		case 0xB000:								// Jump to location nnn + r[0]
			log::Debug("JP V0, nnn\n");
			pc = r[0x0] + nnn;
			break;
		case 0xC000:								// Sets Vx to a random number AND kk
			log::Debug("RND V%X, %X\n", x, kk);
			r[x] = (rand() % 0xFF) & kk;
			pc += 2;
			break;
		case 0xD000: {
			// DXYN: Draws a sprite at coordinate (r[X], r[Y]) that has a width of 8 pixels and a height of N pixels.
			// Each row of 8 pixels is read as bit-coded starting from memory::RAM location I;
			// I value doesn't change after the execution of this instruction.
			// r[0xF] is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
			// and to 0 if that doesn't happen
			log::Debug("DRW V%X, V%X, %X\n", x, y, n);
			uint8_t Vx = r[x];
			uint8_t Vy = r[y];
			uint8_t pixel;
			r[0xF] = 0;
			for (int yline = 0; yline < n; yline++) {
				pixel = memory::RAM[I + yline];
				for (int xline = 0; xline < 8; xline++) { //iterate the bits of pixel from high to low bit,if that pixel is going to set to 0 and the original buffer is 1,then set r[0xF] = 1;
					if (0 != (pixel & (0x80 >> xline))) {
						auto addr = Vx + xline + ((Vy + yline) * 64);
						if (addr >= display::bufferSize) {
							log::Debug("frame buffer size is out of range!");
							continue;
						}
						if (1 == display::frameBuffer[addr]) {
							r[0xF] = 1;
						}
						display::frameBuffer[addr] ^= 1;
					}
				}
			}
			display::drawFlag = true;
			pc += 2;
		}
				   break;

		case 0xE000:
			switch (kk) {
			case 0x009E:					// Skip next instruction if key with the value of Vx is pressed
				log::Debug("SKP V%X\n", x);
				if (keyboard::isKeyPressed(r[x]))
					pc += 4;
				else
					pc += 2;
				break;

			case 0x00A1:					// Skip next instruction if key with the value of Vx isn't pressed
				log::Debug("SKP V%X\n", x);
				if (!keyboard::isKeyPressed(r[x]))
					pc += 4;
				else
					pc += 2;
				break;

			default:
				log::Debug("Unknown opcode [0xE000]: 0x%X\n", opcode);
			}
			break;

		case 0xF000:
			switch (kk) {
			case 0x0007:								// Set Vx = delay timer value
				log::Debug("LD V%X, DT\n", x);
				r[x] = delay_timer;
				pc += 2;
				break;

			case 0x000A: {								// Wait for a key press, store the value of the key in Vx
				log::Debug("LD V%X, K\n", x);
				bool keyPress = false;
				u8 pressedKey = keyboard::getKey();
				if (pressedKey < 0x10) {
					r[x] = pressedKey;
					keyPress = true;
				}
				// If we didn't received a keypress, skip this cycle and try again
				if (!keyPress)
					return;
				pc += 2;
			}
					   break;

			case 0x0015:
				// Set delay timer = Vx.
				log::Debug("LD DT, V%X\n", x);
				delay_timer = r[x];
				pc += 2;
				break;

			case 0x0018:
				//  Set sound timer = Vx
				log::Debug("LD ST, V%X\n", x);
				sound_timer = r[x];
				pc += 2;
				break;

			case 0x001E:
				// Set I = I + Vx
				log::Debug("ADD I, V%X\n", x);
				// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't
				r[0xF] = (I + r[x] > 0xFFF) ? 1 : 0;
				I += r[x];
				pc += 2;
				break;

			case 0x0029:
				// Set I = location of sprite for digit Vx
				// Characters 0-F (in hexadecimal) are represented by a 4x5 font
				log::Debug("LD F, V%X\n", x);
				I = r[x] * 0x5;
				pc += 2;
				break;

			case 0x0033:
				// Store BCD representation of Vx in memory::RAM locations I, I+1, and I+2
				log::Debug("LD B, V%X\n", x);
				memory::RAM[I] = r[x] / 10;
				memory::RAM[I + 1] = (r[x] / 10) % 10;
				memory::RAM[I + 2] = (r[x] % 100) % 10;
				pc += 2;
				break;

			case 0x0055:
				// Store registers V0 through Vx in memory::RAM starting at location I
				log::Debug("LD [I], V%X\n", x);
				for (int i = 0; i <= x; ++i)
					memory::RAM[I + i] = r[i];
				// On the original interpreter, when the operation is done, I = I + X + 1
				I += x + 1;
				pc += 2;
				break;

			case 0x0065:
				// Read registers V0 through Vx from memory::RAM starting at location I
				log::Debug("LD V%X, [I]", x);
				for (int i = 0; i <= x; ++i)
					r[i] = memory::RAM[I + i];
				// On the original interpreter, when the operation is done, I = I + X + 1
				I += x + 1;
				pc += 2;
				break;

			default:
				log::Debug("Unknown opcode [0xF000]: 0x%X\n", opcode);
			}
			break;

		default:
			log::Debug("Unknown opcode: 0x%X\n", opcode);
		}

		// Update timers
		if (delay_timer > 0)
			--delay_timer;
		if (sound_timer > 0) {
			if (1 == sound_timer) {
				Beep(1000, 500);
				log::Debug("BEEP\n");
			}

			--sound_timer;
		}
	}

	void debugStack() {
		for (int idx = 0; idx < 16; ++idx)
			log::Debug("%02x\t", stack[idx]);
	}
}
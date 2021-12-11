#include "parser.h"

namespace disasm {
	instruction parse(u16 val, u16 l) {
		std::uint8_t nibbles[4];
		nibbles[0] = (val & 0xF000u) >> 12u;
		nibbles[1] = (val & 0x0F00u) >> 8u;
		nibbles[2] = (val & 0x00F0u) >> 4u;
		nibbles[3] = (val & 0x000Fu);

		switch (nibbles[0])
		{
		case 0:
			if (nibbles[1] == 0 && nibbles[2] == 0xE) {
				switch (nibbles[3]) {
				case 0:
					return CLS{ {val, l} };    //0x00E0
				case 0xE:
					return RET{ {val, l} };    //0x00EE
				}
			}
			break;
		case 1:
			return JUMPI{ {val, l} };
		case 2:
			return CALLI{ {val, l} };
		case 3:
			return SKEI{ {val, l} };
		case 4:
			return SKNEI{ {val, l} };
		case 5:
			return SKER{ {val, l} };
		case 6:
			return SETI{ {val, l} };
		case 7:
			return ADDI{ {val, l} };
		case 8:
			switch (nibbles[3]) {
			case 0:
				return SETR{ {val, l} };
			case 1:
				return ORR{ {val, l} };
			case 2:
				return ANDR{ {val, l} };
			case 3:
				return XORR{ {val, l} };
			case 4:
				return ADDR{ {val, l} };
			case 5:
				return SUBR{ {val, l} };
			case 6:
				return SHR{ {val, l} };
			case 7:
				return SUBR_R{ {val, l} };
			case 0xE:
				return SHL{ {val, l} };
			}
		case 9:
			return SKNER{ {val, l} };
		case 0xA:
			return ISETI{ {val, l} };
		case 0xB:
			return JUMPIND{ {val, l} };
		case 0xC:
			return RAND{ {val, l} };
		case 0xD:
			return DRAW{ {val, l} };
		case 0xE:
			switch ((nibbles[2] << 4u) | nibbles[3]) {
			case 0x9E:
				return SKEK{ {val, l} };
			case 0xA1:
				return SKNEK{ {val, l} };
			default:
				return UNKNOWN{ { val, l } };
			}
		case 0xF:
			switch ((nibbles[2] << 4u) | nibbles[3]) {
			case 0x07:
				return GETD{ {val, l} };
			case 0x0A:
				return KEY{ {val, l} };
			case 0x15:
				return SETD{ {val, l} };
			case 0x18:
				return SETS{ {val, l} };
			case 0x1E:
				return IADDR{ {val, l} };
			case 0x29:
				return SPRITE{ {val, l} };
			case 0x33:
				return BCD{ {val, l} };
			case 0x55:
				return STORE{ {val, l} };
			case 0x65:
				return LOAD{ {val, l} };
			}
		default:
			return UNKNOWN{ { val, l } };
			break;
		}
		return UNKNOWN{ { val, l } };
	}
}
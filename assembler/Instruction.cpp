//
// Created by william on 2020-06-13.
//

#include "Instruction.h"

namespace assembler {
	Instruction::Instruction(Instruction::InstructionType instructionType, std::optional<unsigned int> x,
		std::optional<unsigned int> y, std::optional<unsigned int> n, std::optional<unsigned int> kk,
		std::optional<unsigned int> nnn, std::optional<std::string> errorMessage,
		unsigned int errorLineNumber, unsigned int nnnn)
		: instructionType(instructionType),
		n(n), x(x), y(y),
		kk(kk), nnn(nnn), errorMessage(errorMessage), errorLineNumber(errorLineNumber),
		nnnn(nnnn) {}

	Instruction Instruction::fromNoParams(Instruction::InstructionType instructionType) {
		return Instruction(instructionType, {}, {}, {}, {}, {}, {});
	}

	Instruction Instruction::fromAddress(InstructionType instructionType, unsigned int nnn) {
		return Instruction(instructionType, {}, {}, {}, {}, nnn, {});
	}

	Instruction Instruction::fromRegister(InstructionType instructionType, unsigned int x) {
		return Instruction(instructionType, x, {}, {}, {}, {}, {});
	}

	Instruction Instruction::fromRegisterByte(InstructionType instructionType, unsigned int x, unsigned int kk) {
		return Instruction(instructionType, x, {}, {}, kk, {}, {});
	}

	Instruction Instruction::fromTwoRegisters(InstructionType instructionType, unsigned int x, unsigned int y) {
		return Instruction(instructionType, x, y, {}, {}, {}, {});
	}

	Instruction Instruction::fromTwoRegistersAndNibble(InstructionType instructionType, unsigned int x, unsigned int y,
		unsigned int n) {
		return Instruction(instructionType, x, y, n, {}, {}, {});
	}

	Instruction Instruction::error(std::string errorMessage, unsigned int lineNumber) {
		return Instruction(Instruction::InstructionType::SyntaxError, {}, {}, {}, {}, {}, errorMessage, lineNumber);
	}

	Instruction Instruction::fromValue(unsigned int value) {
		return Instruction(Instruction::InstructionType::MemoryValue, {}, {}, {}, {}, {}, {}, 0, value);
	}

	unsigned int Instruction::getByteCode() const {
		unsigned int baseCode = 0;
		switch (instructionType) {
		case InstructionType::System:
			baseCode = 0;
			break;
		case InstructionType::Clear:
			baseCode = 0x00E0u;
			break;
		case InstructionType::Return:
			baseCode = 0x00EEu;
			break;
		case InstructionType::Jump:
			baseCode = 0x1000u;
			break;
		case InstructionType::Call:
			baseCode = 0x2000u;
			break;
		case InstructionType::SkipIfEqualByte:
			baseCode = 0x3000u;
			break;
		case InstructionType::SkipIfNotEqualByte:
			baseCode = 0x4000u;
			break;
		case InstructionType::SkipIfEqualRegister:
			baseCode = 0x5000u;
			break;
		case InstructionType::LoadByte:
			baseCode = 0x6000u;
			break;
		case InstructionType::AddByte:
			baseCode = 0x7000u;
			break;
		case InstructionType::LoadRegister:
			baseCode = 0x8000u;
			break;
		case InstructionType::Or:
			baseCode = 0x8001u;
			break;
		case InstructionType::And:
			baseCode = 0x8002u;
			break;
		case InstructionType::Xor:
			baseCode = 0x8003u;
			break;
		case InstructionType::AddRegister:
			baseCode = 0x8004u;
			break;
		case InstructionType::Sub:
			baseCode = 0x8005u;
			break;
		case InstructionType::ShiftRight:
			baseCode = 0x8006u;
			break;
		case InstructionType::SubFrom:
			baseCode = 0x8007u;
			break;
		case InstructionType::ShiftLeft:
			baseCode = 0x800Eu;
			break;
		case InstructionType::SkipIfNotEqualRegister:
			baseCode = 0x9000u;
			break;
		case InstructionType::LoadIndex:
			baseCode = 0xA000u;
			break;
		case InstructionType::JumpTo:
			baseCode = 0xB000u;
			break;
		case InstructionType::Random:
			baseCode = 0xC000u;
			break;
		case InstructionType::Draw:
			baseCode = 0xD000u;
			break;
		case InstructionType::SkipIfKeyPressed:
			baseCode = 0xE09Eu;
			break;
		case InstructionType::SkipIfNotKeyPressed:
			baseCode = 0xE0A1u;
			break;
		case InstructionType::LoadDelayTimer:
			baseCode = 0xF007u;
			break;
		case InstructionType::LoadKey:
			baseCode = 0xF00Au;
			break;
		case InstructionType::SetDelayTimer:
			baseCode = 0xF015u;
			break;
		case InstructionType::SetSoundTimer:
			baseCode = 0xF018u;
			break;
		case InstructionType::AddIndex:
			baseCode = 0xF01Eu;
			break;
		case InstructionType::LoadFontLocation:
			baseCode = 0xF029u;
			break;
		case InstructionType::LoadBinaryCodedDigit:
			baseCode = 0xF033u;
			break;
		case InstructionType::WriteRegisters:
			baseCode = 0xF055u;
			break;
		case InstructionType::ReadRegisters:
			baseCode = 0xF065u;
			break;
		case InstructionType::SyntaxError:
			throw std::runtime_error("Tried to compile error");
			break;
		case InstructionType::MemoryValue:
			baseCode = 0;
			break;
		default:
			throw std::runtime_error("Case not handled");
			break;
		}
		unsigned int byteCode = baseCode;

		if (n.has_value()) {
			byteCode |= n.value();
		}
		if (x.has_value()) {
			byteCode |= x.value() << (2 * 4);
		}
		if (y.has_value()) {
			byteCode |= y.value() << (1 * 4);
		}
		if (kk.has_value()) {
			byteCode |= kk.value();
		}
		if (nnn.has_value()) {
			byteCode |= nnn.value();
		}
		if (nnnn.has_value()) {
			byteCode |= nnnn.value();
		}

		return byteCode;
	}

	Instruction::InstructionType Instruction::getInstructionType() const {
		return instructionType;
	}

	std::optional<std::string> Instruction::getErrorMessage() const {
		return errorMessage;
	}

	unsigned int Instruction::getErrorLineNumber() const {
		return errorLineNumber;
	}
}
//
// Created by william on 2020-06-13.
//

#ifndef CHIP8_ASSEMBLER_INSTRUCTION_H
#define CHIP8_ASSEMBLER_INSTRUCTION_H

#include <cstdint>
#include <optional>

#include "Token.h"

namespace assembler {
    class Instruction {
    public:
        enum class InstructionType {
            System,
            Clear,
            Return,
            Jump,
            Call,
            SkipIfEqualByte,
            SkipIfNotEqualByte,
            SkipIfEqualRegister,
            LoadByte,
            AddByte,
            LoadRegister,
            Or,
            And,
            Xor,
            AddRegister,
            Sub,
            ShiftRight,
            SubFrom,
            ShiftLeft,
            SkipIfNotEqualRegister,
            LoadIndex,
            JumpTo,
            Random,
            Draw,
            SkipIfKeyPressed,
            SkipIfNotKeyPressed,
            LoadDelayTimer,
            LoadKey,
            SetDelayTimer,
            SetSoundTimer,
            AddIndex,
            LoadFontLocation,
            LoadBinaryCodedDigit,
            WriteRegisters,
            ReadRegisters,
            SyntaxError,
            MemoryValue,
        };
    private:

        InstructionType instructionType;
        std::optional<unsigned int> n;
        std::optional<unsigned int> x;
        std::optional<unsigned int> y;
        std::optional<unsigned int> kk;
        std::optional<unsigned int> nnn;
        std::optional<unsigned int> nnnn;
        std::optional<std::string> errorMessage;
        unsigned int errorLineNumber;

        Instruction(InstructionType instructionType, std::optional<unsigned int> x, std::optional<unsigned int> y,
            std::optional<unsigned int> n, std::optional<unsigned int> kk,
            std::optional<unsigned int> nnn, std::optional<std::string> errorMessage,
            unsigned int errorLineNumber = 0,
            unsigned int nnnn = 0);

    public:
        static Instruction fromNoParams(InstructionType instructionType);

        static Instruction fromAddress(InstructionType instructionType, unsigned int nnn);

        static Instruction fromRegister(InstructionType instructionType, unsigned int x);

        static Instruction fromRegisterByte(InstructionType instructionType, unsigned int x, unsigned int kk);

        static Instruction fromTwoRegisters(InstructionType instructionType, unsigned int x, unsigned int y);

        static Instruction
            fromTwoRegistersAndNibble(InstructionType instructionType, unsigned int x, unsigned int y, unsigned int n);

        static Instruction error(std::string errorMessage, unsigned int lineNumber);

        static Instruction fromValue(unsigned int value);

        [[nodiscard]] unsigned int getByteCode() const;

        InstructionType getInstructionType() const;

        std::optional<std::string> getErrorMessage() const;

        unsigned int getErrorLineNumber() const;
    };
}

#endif //CHIP8_ASSEMBLER_INSTRUCTION_H

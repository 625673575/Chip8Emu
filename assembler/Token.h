//
// Created by william on 2020-06-25.
//

#ifndef CHIP8_ASSEMBLER_TOKEN_H
#define CHIP8_ASSEMBLER_TOKEN_H

#include <string>
#include <exception>
#include <sstream>

namespace assembler {
	class Token {
	public:
		enum class TokenType {
			// Instructions
			System,
			Clear,
			Return,
			Jump,
			Call,
			SkipNextIfEqual,
			SkipNextIfNotEqual,
			Load,
			Add,
			Or,
			And,
			Xor,
			Sub,
			SubFrom,
			ShiftRight,
			ShiftLeft,
			Random,
			Draw,
			SkipIfKeyPressed,
			SkipIfNotKeyPressed,
			// Reserved registers
			Index,
			IndexAddress,
			DelayTimer,
			Key,
			SoundTimer,
			Font,
			BinaryCodedDigit,
			// Constants
			Register,
			Address,
			Byte,
			Nibble,
			Word,
			// Assembler
			Section,
			DataSection,
			TextSection,
			Label,
			// Misc.
			Error,
			EndOfFile,
			EndOfLine,
		};

		Token(TokenType tokenType, const std::string& lexeme, const std::string& literal, int lineNumber);

		TokenType getTokenType() const;

		const std::string& getLexeme() const;

		const std::string& getLiteral() const;

		int getLineNumber() const;

		std::string toString() const;

	private:
		const TokenType tokenType;
		const std::string lexeme;
		const std::string literal;
		const unsigned int lineNumber;
	};
}
#endif //CHIP8_ASSEMBLER_TOKEN_H

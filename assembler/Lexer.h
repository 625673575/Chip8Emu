//
// Created by william on 2020-06-13.
//

#ifndef CHIP8_ASSEMBLER_LEXER_H
#define CHIP8_ASSEMBLER_LEXER_H

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

#include "Token.h"

namespace assembler {
	class Lexer {
	public:
		std::vector<Token> scan(const std::string& code);

	private:
		const std::map<std::string, Token::TokenType> STRINGS_TO_TOKENS = {
				{"sys",     Token::TokenType::System},
				{"cls",     Token::TokenType::Clear},
				{"ret",     Token::TokenType::Return},
				{"jp",      Token::TokenType::Jump},
				{"call",    Token::TokenType::Call},
				{"se",      Token::TokenType::SkipNextIfEqual},
				{"sne",     Token::TokenType::SkipNextIfNotEqual},
				{"ld",      Token::TokenType::Load},
				{"add",     Token::TokenType::Add},
				{"or",      Token::TokenType::Or},
				{"and",     Token::TokenType::And},
				{"xor",     Token::TokenType::Xor},
				{"sub",     Token::TokenType::Sub},
				{"subn",    Token::TokenType::SubFrom},
				{"shr",     Token::TokenType::ShiftRight},
				{"shl",     Token::TokenType::ShiftLeft},
				{"rnd",     Token::TokenType::Random},
				{"drw",     Token::TokenType::Draw},
				{"skp",     Token::TokenType::SkipIfKeyPressed},
				{"sknp",    Token::TokenType::SkipIfNotKeyPressed},
				{"i",       Token::TokenType::Index},
				{"[i]",     Token::TokenType::IndexAddress},
				{"dt",      Token::TokenType::DelayTimer},
				{"k",       Token::TokenType::Key},
				{"st",      Token::TokenType::SoundTimer},
				{"f",       Token::TokenType::Font},
				{"b",       Token::TokenType::BinaryCodedDigit},
				{"section", Token::TokenType::Section},
				{".data",   Token::TokenType::DataSection},
				{".text",   Token::TokenType::TextSection},
		};
	};
}

#endif //CHIP8_ASSEMBLER_LEXER_H

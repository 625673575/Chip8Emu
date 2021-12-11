//
// Created by william on 2020-06-13.
//

#include "Lexer.h"

namespace assembler {
	bool isHexa(char c) {
		return std::isdigit(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
	}

	std::vector<Token> Lexer::scan(const std::string& code) {
		const unsigned int codeLength = code.size();
		unsigned int start = 0;
		unsigned int current = 0;
		unsigned int numLine = 1;

		std::vector<Token> tokens;
		while (current < codeLength) {
			unsigned int end = current;
			const char c = code[current];
			if (c == ',' || c == ' ' || c == '\n' || current == codeLength - 1) {
				if (start - current == 0) {
					current++;
					start = current;
					if (c == '\n') {
						numLine += 1;
					}
					continue;
				}
				// Handle end of file
				if (current == codeLength - 1 && code[current] != '\n') {
					end += 1;
				}
				// end of lexeme
				std::string lexeme = code.substr(start, end - start);
				std::transform(std::begin(lexeme), std::end(lexeme), std::begin(lexeme),
					[](unsigned char x) { return std::tolower(x); });

				const auto pos = STRINGS_TO_TOKENS.find(lexeme);
				if (pos == STRINGS_TO_TOKENS.cend()) {
					// Token not a keyword
					if (lexeme.starts_with(';')) {
						// Comment
						while (current < codeLength) {
							++current;
							if (code[current] == '\n') {
								tokens.push_back(Token(Token::TokenType::EndOfLine, "", "", numLine));
								numLine++;
								break;
							}
						}
					}
					else if (lexeme.starts_with('v')) {
						if (lexeme.size() != 2 || !isHexa(lexeme[1])) {
							tokens.push_back(Token(Token::TokenType::Error, lexeme, "Rx, x is supposed to be a hexa number",
								numLine));
						}
						tokens.push_back(
							Token(Token::TokenType::Register, lexeme, lexeme.substr(1, lexeme.size() - 1), numLine));
					}
					else if (lexeme.starts_with(":")) {
						if (lexeme.size() <= 1) {
							tokens.push_back(Token(Token::TokenType::Error, lexeme, "Empty label", numLine));
						}
						tokens.push_back(
							Token(Token::TokenType::Label, lexeme, lexeme.substr(1, lexeme.size() - 1), numLine));
					}
					else if (lexeme.starts_with('#') && std::all_of(std::begin(lexeme) + 1, std::end(lexeme), isHexa)) {
						const auto size = lexeme.size() - 1;
						const auto literal = lexeme.substr(1, size);
						switch (size) {
						case 1:
							tokens.push_back(
								Token(Token::TokenType::Nibble, lexeme, literal, numLine));
							break;
						case 2:
							tokens.push_back(
								Token(Token::TokenType::Byte, lexeme, literal, numLine));
							break;
						case 3:
							tokens.push_back(
								Token(Token::TokenType::Address, lexeme, literal, numLine));
							break;
						case 4:
							tokens.push_back(
								Token(Token::TokenType::Word, lexeme, literal, numLine));
							break;
						default:
							tokens.push_back(Token(Token::TokenType::Error, lexeme, "Invalid constant value", numLine));
						}
					}
					else {
						tokens.push_back(Token(Token::TokenType::Error, lexeme, "Invalid token", numLine));
					}
				}
				else {
					const auto type = pos->second;
					tokens.push_back(Token(type, lexeme, "", numLine));
				}
				start = current + 1;
				if (c == '\n') {
					tokens.push_back(Token(Token::TokenType::EndOfLine, "", "", numLine));
					++numLine;
				}
			}
			++current;
		}
		tokens.push_back(Token(Token::TokenType::EndOfFile, "", "", numLine));

		return tokens;
	}
}
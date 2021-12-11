//
// Created by william on 2020-06-25.
//

#include <stdexcept>
#include "Token.h"

namespace assembler {
	Token::Token(Token::TokenType tokenType, const std::string& lexeme, const std::string& literal, int lineNumber)
		: tokenType(tokenType), lexeme(lexeme), literal(literal), lineNumber(lineNumber) {}

	Token::TokenType Token::getTokenType() const {
		return tokenType;
	}

	const std::string& Token::getLexeme() const {
		return lexeme;
	}

	const std::string& Token::getLiteral() const {
		return literal;
	}

	int Token::getLineNumber() const {
		return lineNumber;
	}

	std::string Token::toString() const {
		std::ostringstream out;
		switch (tokenType) {
		case TokenType::System:
			return "SYS";
			break;
		case TokenType::Clear:
			return "CLS";
			break;
		case TokenType::Return:
			return "RET";
			break;
		case TokenType::Jump:
			return "JP";
			break;
		case TokenType::Call:
			return "CALL";
			break;
		case TokenType::SkipNextIfEqual:
			return "SE";
			break;
		case TokenType::SkipNextIfNotEqual:
			return "SNE";
			break;
		case TokenType::Load:
			return "LD";
			break;
		case TokenType::Add:
			return "ADD";
			break;
		case TokenType::Or:
			return "OR";
			break;
		case TokenType::And:
			return "AND";
			break;
		case TokenType::Xor:
			return "XOR";
			break;
		case TokenType::Sub:
			return "SUB";
			break;
		case TokenType::SubFrom:
			return "SUBN";
			break;
		case TokenType::ShiftRight:
			return "SHR";
			break;
		case TokenType::ShiftLeft:
			return "SHL";
			break;
		case TokenType::Random:
			return "RND";
			break;
		case TokenType::Draw:
			return "DRW";
			break;
		case TokenType::SkipIfKeyPressed:
			return "SKP";
			break;
		case TokenType::SkipIfNotKeyPressed:
			return "SKNP";
			break;
		case TokenType::Index:
			return "I";
			break;
		case TokenType::IndexAddress:
			out << "[" << literal << "]";
			return out.str();
			break;
		case TokenType::DelayTimer:
			return "DT";
			break;
		case TokenType::Key:
			return "K";
			break;
		case TokenType::SoundTimer:
			return "ST";
			break;
		case TokenType::Font:
			return "F";
			break;
		case TokenType::BinaryCodedDigit:
			return "B";
			break;
		case TokenType::Register:
			out << "R" << literal;
			return out.str();
			break;
		case TokenType::Address:
		case TokenType::Byte:
		case TokenType::Nibble:
		case TokenType::Word:
			out << "#" << literal;
			return out.str();
			break;
		case TokenType::Section:
			return "SECTION";
			break;
		case TokenType::DataSection:
			return ".data";
			break;
		case TokenType::TextSection:
			return ".text";
			break;
		case TokenType::Label:
			out << ":" << literal;
			return out.str();
			break;
		case TokenType::Error:
			out << "Error on line (" << lineNumber;
			out << "): " << literal;
			return out.str();
			break;
		case TokenType::EndOfFile:
			return "EOF";
			break;
		case TokenType::EndOfLine:
			return "EOL";
			break;
		default:
			throw std::runtime_error("TokenType not handled");
		}
	}
}

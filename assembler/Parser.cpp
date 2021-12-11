//
// Created by william on 2020-06-29.
//

#include "Parser.h"

namespace assembler {
	std::vector<Instruction> Parser::parse(const std::vector<Token>& tokens) {
		using TT = Token::TokenType;
		using IT = Instruction::InstructionType;

		std::map<std::string, unsigned int> labelToAddress;
		{
			const unsigned int INSTRUCTION_SIZE = 2;
			unsigned int address = 0x200;
			bool empty = true;
			bool startOfLine = true;
			for (size_t i = 0; i < tokens.size(); ++i) {
				const auto t = tokens[i];
				if (t.getTokenType() != TT::EndOfLine && t.getTokenType() != TT::EndOfFile) {
					if (t.getTokenType() == TT::Label) {
						if (startOfLine) {
							labelToAddress.insert({ t.getLiteral(), address });
							continue;
						}
					}
					empty = false;
					startOfLine = false;
				}
				else {
					if (!empty) {
						address += INSTRUCTION_SIZE;
						empty = true;
						startOfLine = true;
					}
				}
			}
		}

		std::vector<Instruction> instructions;

		size_t i = 0;
		const auto n = tokens.size();
		while (i < n) {
			std::vector<Token> currentLine;
			{
				while (tokens[i].getTokenType() != TT::EndOfLine && tokens[i].getTokenType() != TT::EndOfFile) {
					currentLine.push_back(tokens[i]);
					++i;
				}
			}

			if (currentLine.size() > 0) {
				const Token firstToken = currentLine[0];
				const size_t numToken = currentLine.size();

				if (numToken == 1) {
					switch (firstToken.getTokenType()) {
					case TT::Clear:
						instructions.push_back(Instruction::fromNoParams(IT::Clear));
						break;
					case TT::Return:
						instructions.push_back(Instruction::fromNoParams(IT::Return));
						break;
					case TT::Label:
						// Do nothing, already found address
						break;
					case TT::Nibble:
					case TT::Byte:
					case TT::Word: {
						unsigned int value = std::stoi(firstToken.getLiteral(), nullptr, 16);
						instructions.push_back(Instruction::fromValue(value));
						break;
					}
					default:
						instructions.push_back(error(firstToken.getLexeme() + " is expecting one or more arguments",
							firstToken.getLineNumber()));
						break;
					}
				}
				else if (numToken == 2) {
					const Token secondToken = currentLine[1];
					if (secondToken.getTokenType() == TT::Address || secondToken.getTokenType() == TT::Label) {
						unsigned int nnn;
						if (secondToken.getTokenType() == TT::Label) {
							nnn = labelToAddress.at(secondToken.getLiteral());
						}
						else {
							nnn = std::stoi(secondToken.getLiteral(), nullptr, 16);
						}

						switch (firstToken.getTokenType()) {
						case TT::System:
							instructions.push_back(Instruction::fromAddress(IT::System, nnn));
							break;
						case TT::Jump:
							instructions.push_back(Instruction::fromAddress(IT::Jump, nnn));
							break;
						case TT::Call:
							instructions.push_back(Instruction::fromAddress(IT::Call, nnn));
							break;
						default:
							instructions.push_back(error("Invalid argument, found 2 tokens, second one is nnn",
								firstToken.getLineNumber()));
							break;
						}
					}
					else if (secondToken.getTokenType() == TT::Register) {
						unsigned int x = std::stoi(secondToken.getLiteral(), nullptr, 16);

						switch (firstToken.getTokenType()) {
						case TT::ShiftRight:
							instructions.push_back(Instruction::fromAddress(IT::ShiftRight, x));
							break;
						case TT::ShiftLeft:
							instructions.push_back(Instruction::fromAddress(IT::ShiftLeft, x));
							break;
						case TT::SkipIfKeyPressed:
							instructions.push_back(Instruction::fromAddress(IT::SkipIfKeyPressed, x));
							break;
						case TT::SkipIfNotKeyPressed:
							instructions.push_back(Instruction::fromAddress(IT::SkipIfNotKeyPressed, x));
							break;
						default:
							instructions.push_back(error("Invalid argument, 2 tokens, x", firstToken.getLineNumber()));
							break;
						}
					}
					else {
						instructions.push_back(error("Invalid argument, bad 2 tokens", firstToken.getLineNumber()));
					}
				}
				else if (numToken == 3) {
					const Token secondToken = currentLine[1];
					const Token thirdToken = currentLine[2];

					if (secondToken.getTokenType() == TT::Register) {
						unsigned int x = std::stoi(secondToken.getLiteral(), nullptr, 16);
						if (firstToken.getTokenType() == TT::Jump) {
							if (x == 0 &&
								(thirdToken.getTokenType() == TT::Address || thirdToken.getTokenType() == TT::Label)) {
								unsigned int nnn;
								if (thirdToken.getTokenType() == TT::Label) {
									nnn = labelToAddress.at(thirdToken.getLiteral());
								}
								else {
									nnn = std::stoi(thirdToken.getLiteral(), nullptr, 16);
								}
								instructions.push_back(Instruction::fromAddress(IT::JumpTo, nnn));
							}
							else {
								instructions.push_back(error("Expected address", firstToken.getLineNumber()));
							}
						}
						else if (thirdToken.getTokenType() == TT::Register) {
							unsigned int y = std::stoi(thirdToken.getLiteral(), nullptr, 16);

							switch (firstToken.getTokenType()) {
							case TT::Load:
								instructions.push_back(Instruction::fromTwoRegisters(IT::LoadRegister, x, y));
								break;
							case TT::SkipNextIfEqual:
								instructions.push_back(Instruction::fromTwoRegisters(IT::SkipIfEqualRegister, x, y));
								break;
							case TT::Or:
								instructions.push_back(Instruction::fromTwoRegisters(IT::Or, x, y));
								break;
							case TT::And:
								instructions.push_back(Instruction::fromTwoRegisters(IT::And, x, y));
								break;
							case TT::Xor:
								instructions.push_back(Instruction::fromTwoRegisters(IT::Xor, x, y));
								break;
							case TT::Add:
								instructions.push_back(Instruction::fromTwoRegisters(IT::AddRegister, x, y));
								break;
							case TT::Sub:
								instructions.push_back(Instruction::fromTwoRegisters(IT::Sub, x, y));
								break;
							case TT::SubFrom:
								instructions.push_back(Instruction::fromTwoRegisters(IT::SubFrom, x, y));
								break;
							case TT::SkipNextIfNotEqual:
								instructions.push_back(Instruction::fromTwoRegisters(IT::SkipIfNotEqualRegister, x, y));
								break;
							default:
								instructions.push_back(
									error("Invalid argument, 3 tokens, 3rd = x", firstToken.getLineNumber()));
								break;
							}
						}
						else if (thirdToken.getTokenType() == TT::Byte || thirdToken.getTokenType() == TT::Nibble) {
							unsigned int kk = std::stoi(thirdToken.getLiteral(), nullptr, 16);

							switch (firstToken.getTokenType()) {
							case TT::SkipNextIfEqual:
								instructions.push_back(Instruction::fromRegisterByte(IT::SkipIfEqualByte, x, kk));
								break;
							case TT::SkipNextIfNotEqual:
								instructions.push_back(Instruction::fromRegisterByte(IT::SkipIfNotEqualByte, x, kk));
								break;
							case TT::Load:
								instructions.push_back(Instruction::fromRegisterByte(IT::LoadByte, x, kk));
								break;
							case TT::Add:
								instructions.push_back(Instruction::fromRegisterByte(IT::AddByte, x, kk));
								break;
							case TT::Random:
								instructions.push_back(Instruction::fromRegisterByte(IT::Random, x, kk));
								break;
							default:
								instructions.push_back(
									error("Invalid argument, 3 tokens, 3rd = kk", firstToken.getLineNumber()));
								break;
							}
						}
						else if (thirdToken.getTokenType() == TT::DelayTimer) {
							if (firstToken.getTokenType() == TT::Load) {
								instructions.push_back(Instruction::fromRegister(IT::LoadDelayTimer, x));
							}
							else {
								instructions.push_back(error("Invalid instruction, expected Load Delay Timer instruction",
									firstToken.getLineNumber()));
							}
						}
						else if (thirdToken.getTokenType() == TT::Key) {
							if (firstToken.getTokenType() == TT::Load) {
								instructions.push_back(Instruction::fromRegister(IT::LoadKey, x));
							}
							else {
								instructions.push_back(error("Invalid instruction, expected Load Key instruction",
									firstToken.getLineNumber()));
							}
						}
						else {
							instructions.push_back(error("Invalid argument, 3 tokens", firstToken.getLineNumber()));
						}
					}
					else if (secondToken.getTokenType() == TT::Index) {
						if (firstToken.getTokenType() == TT::Load &&
							(thirdToken.getTokenType() == TT::Address || thirdToken.getTokenType() == TT::Label)) {

							unsigned int nnn;
							if (thirdToken.getTokenType() == TT::Label) {
								nnn = labelToAddress.at(thirdToken.getLiteral());
							}
							else {
								nnn = std::stoi(thirdToken.getLiteral(), nullptr, 16);
							}

							instructions.push_back(Instruction::fromAddress(IT::LoadIndex, nnn));
						}
						else if (firstToken.getTokenType() == TT::Add && thirdToken.getTokenType() == TT::Register) {
							unsigned int x = std::stoi(thirdToken.getLiteral(), nullptr, 16);
							instructions.push_back(Instruction::fromRegister(IT::AddIndex, x));
						}
						else {
							instructions.push_back(error("Invalid instruction, expected Load Inder or Add Index",
								firstToken.getLineNumber()));
						}
					}
					else if (secondToken.getTokenType() == TT::Load) {
						if (thirdToken.getTokenType() == TT::Register) {
							unsigned int x = std::stoi(thirdToken.getLiteral(), nullptr, 16);
							if (secondToken.getTokenType() == TT::DelayTimer) {
								instructions.push_back(Instruction::fromRegister(IT::SetDelayTimer, x));
							}
							else if (secondToken.getTokenType() == TT::SoundTimer) {
								instructions.push_back(Instruction::fromRegister(IT::SetSoundTimer, x));
							}
							else if (secondToken.getTokenType() == TT::SoundTimer) {
								instructions.push_back(Instruction::fromRegister(IT::LoadFontLocation, x));
							}
							else if (secondToken.getTokenType() == TT::SoundTimer) {
								instructions.push_back(Instruction::fromRegister(IT::LoadBinaryCodedDigit, x));
							}
							else if (secondToken.getTokenType() == TT::IndexAddress) {
								instructions.push_back(Instruction::fromRegister(IT::WriteRegisters, x));
							}
							else {
								instructions.push_back(error("Invalid argument, 3 token, LD", firstToken.getLineNumber()));
							}
						}
						else if (secondToken.getTokenType() == TT::Register &&
							thirdToken.getTokenType() == TT::IndexAddress) {
							unsigned int x = std::stoi(secondToken.getLiteral(), nullptr, 16);
							instructions.push_back(Instruction::fromRegister(IT::ReadRegisters, x));
						}
						else {
							instructions.push_back(error("Invalid instruction, 3 tokens", firstToken.getLineNumber()));
						}
					}
				}
				else if (numToken == 4) {
					const Token secondToken = currentLine[1];
					const Token thirdToken = currentLine[2];
					const Token fourthToken = currentLine[3];

					if (firstToken.getTokenType() == TT::Draw && secondToken.getTokenType() == TT::Register
						&& thirdToken.getTokenType() == TT::Register && fourthToken.getTokenType() == TT::Nibble) {
						unsigned int x = std::stoi(secondToken.getLiteral(), nullptr, 16);
						unsigned int y = std::stoi(thirdToken.getLiteral(), nullptr, 16);
						unsigned int n = std::stoi(fourthToken.getLiteral(), nullptr, 16);
						instructions.push_back(Instruction::fromTwoRegistersAndNibble(IT::Draw, x, y, n));
					}
					else {
						instructions.push_back(error("Invalid instruction, 4 tokens", firstToken.getLineNumber()));
					}
				}
			}
			++i;
		}

		return instructions;
	}

	Instruction Parser::error(std::string errorMessage, unsigned int lineNumber) {
		return Instruction::error(errorMessage, lineNumber);
	}
}
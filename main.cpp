#include <iostream>
#include <fstream>
#include "cpu/cpu.h"
#include "display/display.h"
#include "input/keyboard.h"
#include "framework/disasm_window.h"
#include "framework/debugger_window.h"
#include "assembler/Lexer.h"
#include "assembler/Parser.h"
#pragma warning(disable : 6387)

u8* buffer = NULL;
long bufferSize = 0;
bool running = false;

void free_data() {
	if (buffer != NULL)
		free(buffer);
}

void read_buffer(const char* path) {
	FILE* file = fopen(path, "rb");
	if (file == NULL) {
		fprintf(stderr, "WHERES THE FILE?!");
		exit(1);
	}
	if (buffer != NULL)
		free(buffer);
	fseek(file, 0l, SEEK_END);
	bufferSize = ftell(file);
	fseek(file, 0l, SEEK_SET);

	buffer = (uint8_t*)malloc(bufferSize);
	fread(buffer, bufferSize, 1, file);
	fclose(file);
}

void run_program(const char* path) {
	read_buffer(path);
	std::cout << "Running " << path << std::endl;

	display::init();
	display::Display display;
	char title[] = "Chip8";
	display.videoInit(title, SCREEN_WIDTH, SCREEN_HEIGHT, 20);

	keyboard::init();

	cpu::loadProgram(buffer, bufferSize);

	running = true;
	while (running) {
		cpu::emulateCycle();
		if (display::drawFlag) {
			display.drawVideo(display::frameBuffer);
			//display.drawTexture(display::frameBuffer);
			display::drawFlag = false;
		}
		keyboard::checkKeyInput();
		auto keyinput = keyboard::getKey();
		running = keyinput != keyboard::shutdownKey;
	}
}

void disassembly(const char* path) {
	read_buffer(path);
	std::cout << "Disasemmbly " << path << std::endl;
	auto disasm_window = disasm_window::create();
	disasm_window.disassembly(buffer, bufferSize);
	disasm_window.show();
}

void assemble(const char* input, const char* output) {
	std::ifstream file(input);
	std::string source;

	file.seekg(0, std::ios::end);
	source.reserve(file.tellg());
	file.seekg(0, std::ios::beg);

	source.assign(std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>());
	file.close();

	assembler::Lexer lexer;
	const std::vector<assembler::Token> tokens = lexer.scan(source);

	bool hadTokenError = false;
	for (const auto t : tokens) {
		if (t.getTokenType() == assembler::Token::TokenType::Error) {
			std::cout << "Error occured when scanning source code" << std::endl;
			std::cout << t.getLiteral() << std::endl;
			std::cout << "On line " << t.getLineNumber() << std::endl;
			hadTokenError = true;
		}
	}

	if (hadTokenError) {
		std::cout << "Tokenization error" << std::endl;
		std::exit(1);
	}

	auto parser = assembler::Parser();
	const auto instructions = parser.parse(tokens);

	bool hadParsingError = false;
	for (const auto i : instructions) {
		if (i.getInstructionType() == assembler::Instruction::InstructionType::SyntaxError) {
			std::cout << "A syntax error occured when parsing source code" << std::endl;
			std::cout << i.getErrorMessage().value() << std::endl;
			std::cout << "On line " << i.getErrorLineNumber() << std::endl;
			hadParsingError = true;
		}
	}

	if (hadParsingError) {
		std::cout << "Parsing error" << std::endl;
		std::exit(1);
	}

	std::ofstream out(output, std::ios::binary);
	for (const auto i : instructions) {
		unsigned int bytecode = i.getByteCode();
		unsigned char higher = bytecode >> (8);
		unsigned char lower = bytecode & 0xFF;
		out << higher << lower;
	}
	out.close();
}

void debug_run() {
	auto debugger = debugger_window::create();
	debugger.show();
}

int main(int argc, char* argv[]) {
	switch (argc)
	{
	case 2://无参数直接运行ch8程序
		run_program(argv[1]);
		break;
	case 3://反汇编ch8程序
		if (strcmp(argv[1], "disasm") == 0)
			disassembly(argv[2]);
		break;
	case 4:
		if (strcmp(argv[1], "asm") == 0)
			assemble(argv[2], argv[3]);
		break;
	default:
		debug_run();
		break;
	}
	free_data();
	return 0;
}
# chip8

#### Description
I believe that a lot of programmers have thought of developing a game console simulator, you may had try to compile some open source projects, but scare and retreat due to the requirements of huge knowledge , CHIP8 simulator is one of the most simple simulation of the CPU ,the core functional instruction is less than 30 but can simulate the whole operation of the CPU, if you want to develop simulator, this will be the best choices to learn the principle of simulation.
This is more of a learning project, where you can learn about CPU simulation, assembly,debugger, compiler principles, etc
I have finish the implementation of emulator, disassembly tool, debugging tool, and assembly compiler,using the latest C++20 features

#### Software Architecture

modules:
1. common - a set of functions that are generic
2. framework - primarily used for rendering,rely on libraries [IMGUI,SDL2]
3. cpu,input,display,memory - responsible for chip8's analog operations
4. assembler - responsible for compiling the output ROM of the assembler
5. disassemble - disassemble the ROM
6. debugger - implement the debugger part, relying on disassemble

#### Installation

Install Visual Studio 2017 or above,the project setting already add handling of build events, which can automatically configure dependencies,just open and run,if you encounter any issues , pull questions let me know
CMake build system will be considered in the future, and other developers are welcome to add cross-platform support

#### Instructions

1. With no parameters,will run debugger with Window
2. With one parameter [path to ROM],will run ROM directly
3. with "disasm bin.ch8",will disassemble bin.ch8
4. with "asm ball.c8 bin.ch8",will compiles ball.c8 to output ROM file bin.ch8

#### Future plans

1. Add C style programming language support to generate assembly code
2. Add front-end and front-end code generation based on LLVM 
3. Extend bytecode functionality to implement a more complex console emulator and equip it with a full suite of development tool chains

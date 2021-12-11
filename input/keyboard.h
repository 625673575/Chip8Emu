#pragma once
#include "../common/types.h"
#ifdef _WIN32
#include<SDL.h>
#else
#include<SDL2/SDL.h>
#endif
namespace keyboard
{
	static const u8 defaultKey = 0xfe;
	static const u8 shutdownKey = 0xff;
	extern void checkKeyInput();	
	extern void handleEvent(const SDL_Event& e);
	extern bool isKeyPressed(u8 pressedKey); 
	extern u8 getKey();
	extern void init();
};


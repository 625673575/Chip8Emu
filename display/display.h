#pragma once
#define SDL2_DISPLAY
//#define GLEW_DISPLAY
#include "../common/types.h"
#ifdef  SDL2_DISPLAY
#include "display_SDL2_Impl.h"
#endif 
#ifdef  GLEW_DISPLAY
#include "display_glew_Impl.h"
#endif 

#define SCREEN_WIDTH 0x40
#define SCREEN_HEIGHT 0x20
#define SCALE 20

namespace display {
	constexpr int bufferSize = SCREEN_WIDTH * SCREEN_HEIGHT;
	extern bool drawFlag;
	extern u8 videoBuffer[SCREEN_WIDTH * SCREEN_HEIGHT]; //video buffer
	extern u8 frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT]; //one for real display
	extern void init(bool test = false);
#ifdef  SDL2_DISPLAY
	using Display = display::display_SDL2_Impl;
#endif //  SDL2_DISPLAY
#ifdef  GLEW_DISPLAY
	using Display = display::display_glew_Impl;
#endif //  SDL2_DISPLAY

}

#pragma once
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<string>
#include<stdint.h>


#include "../framework/window.h"
using namespace framework;

namespace display {
	class display_SDL2_Impl :public window {
	private:
		SDL_Surface* screenSurface;
		SDL_Rect* pixelRect;

		int screenWidth;
		int screenHeight;
		int pixelScale;
	public:
		~display_SDL2_Impl();
		void videoInit(char* str, int w, int h, int scale, bool hidden = false);
		void drawVideo(uint8_t* videoBuffer);
	};

}


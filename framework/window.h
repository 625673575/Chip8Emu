#pragma once

#ifdef __linux__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/mman.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#ifdef __cplusplus
extern "C"
#endif
#endif

#pragma warning(disable:4018; disable:4996; disable:4244)

#ifdef _WIN32
#include<SDL.h>
#include <SDL_types.h>
#else
#include<SDL2/SDL.h>
#include<SDL2/SDL_mixer.h>
#endif

#include<stdlib.h>
#include<stdio.h>
#include<string>

namespace framework {

	class window
	{
	protected:
		SDL_Window* win;
		SDL_Renderer* renderer;
		bool init;
		int width;
		int height;

		mutable char a0[100];
		const char* a1 = ": cpu speed=";
		const char* a2 = " fps=";
		const char* a3 = " frametime=";
		const char* a4 = "ms";

	public:
		window();
		void setWindowVisible(bool show);
		bool createWindow(const char* title, int w, int h, int usageFlag, int rendererFlag);
		void updateTitle(char* str, int cpuspeed, int fps, int frametime) const;
	};

}

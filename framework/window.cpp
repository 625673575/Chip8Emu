#include "window.h"

namespace framework {
	window::window() :init(false), win(nullptr), width(640), height(480)
	{
		SDL_Init(SDL_INIT_VIDEO);
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			// Display error message
			printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
			return;
		}
		init = true;
	}

	void window::setWindowVisible(bool show)
	{
		if (show)
			SDL_ShowWindow(win);
		else
			SDL_HideWindow(win);
	}

	bool window::createWindow(const char* title, int w, int h, int usageFlag, int rendererFlag)
	{
		if (!init)return false;
		height = h;
		width = w;

		win = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, usageFlag);
		renderer = SDL_CreateRenderer(win, -1, rendererFlag);
		if (win == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}

		if (renderer == NULL)
		{
			printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		return win != NULL;
	}
	void window::updateTitle(char* str, int cpuspeed, int fps, int frametime) const
	{
		using namespace std;


		strcpy(a0, str);
		strcat(a0, a1);
		strcat(a0, to_string(cpuspeed).c_str());
		//strcat(a0, a5);
		strcat(a0, a2);
		strcat(a0, to_string(fps).c_str());
		//strcat(a0, a5);
		strcat(a0, a3);
		strcat(a0, to_string(frametime).c_str());
		//strcat(a0, a5);
		strcat(a0, a4);

		//printf("%s\n", a0);

		SDL_SetWindowTitle(win, a0);
	}
}

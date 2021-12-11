#include "display_SDL2_Impl.h"
namespace display {
	display_SDL2_Impl::~display_SDL2_Impl()
	{
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(renderer);
	}
	void display_SDL2_Impl::videoInit(char* str, int w, int h, int scale,bool hidden) {
		screenWidth = w;
		screenHeight = h;
		pixelScale = scale;

		//SDL stuff
		SDL_Init(SDL_INIT_VIDEO);

		bool ok = createWindow(str,w * scale, h * scale, (hidden ? (int)SDL_WINDOW_HIDDEN : (int)SDL_WINDOW_SHOWN), SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (!ok)return;

		pixelRect = new SDL_Rect[w * h];

		int scan = 0;
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				scan = w * y + x;
				pixelRect[scan].x = x * scale;
				pixelRect[scan].y = y * scale;
				pixelRect[scan].w = scale;
				pixelRect[scan].h = scale;
			}

		}
	}

	void display_SDL2_Impl::drawVideo(uint8_t* videoBuffer) {

		int scan = 0;
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer); //clear to blackscreen
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
		//for (int w = 0; w < screenWidth; w++) {
		//	SDL_RenderDrawLine(renderer, w * pixelScale, 0, w * pixelScale, screenHeight * pixelScale);
		//}
		//for (int h = 0; h < screenHeight; h++) {
		//	SDL_RenderDrawLine(renderer, 0, h * pixelScale, screenWidth * pixelScale, h * pixelScale);
		//}
		for (int y = 0; y < screenHeight; y++) {
			for (int x = 0; x < screenWidth; x++) {
				scan = screenWidth * y + x;
				if (videoBuffer[scan] > 0)
					SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				else 
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

				SDL_RenderFillRect(renderer, &pixelRect[scan]);
			}
		}

		SDL_RenderPresent(renderer);
	}
}
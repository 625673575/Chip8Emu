#include "keyboard.h"
namespace keyboard
{
	u8 pressedKey;

	inline u8 getKey() { return pressedKey; }
	void handleEvent(const SDL_Event& e) {
		if (e.type == SDL_QUIT) pressedKey = shutdownKey;
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_x: pressedKey = 0x0;
				break;
			case SDLK_1: pressedKey = 0x1;
				break;
			case SDLK_2: pressedKey = 0x2;
				break;
			case SDLK_3: pressedKey = 0x3;
				break;
			case SDLK_q: pressedKey = 0x4;
				break;
			case SDLK_w: pressedKey = 0x5;
				break;
			case SDLK_e: pressedKey = 0x6;
				break;
			case SDLK_a: pressedKey = 0x7;
				break;
			case SDLK_s: pressedKey = 0x8;
				break;
			case SDLK_d: pressedKey = 0x9;
				break;
			case SDLK_z: pressedKey = 0xa;
				break;
			case SDLK_c: pressedKey = 0xb;
				break;
			case SDLK_4: pressedKey = 0xc;
				break;
			case SDLK_r: pressedKey = 0xd;
				break;
			case SDLK_f: pressedKey = 0xe;
				break;
			case SDLK_v: pressedKey = 0xf;
				break;
			}
		}
		if (e.type == SDL_KEYUP) pressedKey = defaultKey;
	}
	void checkKeyInput() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			handleEvent(e);
		}

	}
	void init() { pressedKey = defaultKey; }

	inline bool isKeyPressed(u8 key) {
		return pressedKey == key;
	}
}
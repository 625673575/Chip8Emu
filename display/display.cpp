#include "display.h"

namespace display {
	bool drawFlag = true; //flag,true will redraw the surface,after drawing ,flag is set to false 
	uint8_t videoBuffer[bufferSize]; //video buffer
	uint8_t frameBuffer[bufferSize]; //one for real display
	void init(bool test)
	{
		memset(videoBuffer, 0, bufferSize);
		memset(frameBuffer, 0, bufferSize);
		if (test) {
			for (int i = 0; i < sizeof(display::frameBuffer); i++)
				display::frameBuffer[i] = (i % 2 == 0 && (i / SCREEN_WIDTH) % 2 == 0) ? 0 : 1;
		}
		drawFlag = true;
	}
}
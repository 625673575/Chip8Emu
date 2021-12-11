#pragma once

#include <GL/glew.h>
#include <SDL_opengl.h>

#include "../framework/window.h"
using namespace framework;

namespace display {
	class display_glew_Impl:public window {
	public:
		~display_glew_Impl();
		void videoInit(char* str, int w, int h, int scale,bool hidden=false);
		void drawVideo(uint8_t* videoBuffer);
		void drawTexture(uint8_t* videoBuffer);
	private:
		SDL_GLContext glContext;
		SDL_Event sdlEvent;
		int screenWidth;
		int screenHeight;
		void draw_quad(int x, int y) const; 
		
		GLuint fbo = 0;        // FBO对象的句柄
		GLuint depthbuffer = 0;
		GLuint rendertarget = 0;        // 纹理对象的句柄
		void setupResource();
	};

}


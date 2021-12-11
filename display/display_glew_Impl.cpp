#include "display_glew_Impl.h"

namespace display {
	display_glew_Impl::~display_glew_Impl()
	{
		SDL_GL_DeleteContext(glContext);
	}
	void display_glew_Impl::videoInit(char* str, int w, int h, int scale, bool hidden)
	{
		screenWidth = w;
		screenHeight = h;
		//Use OpenGL 3.1 core
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		bool ok = createWindow(str, w * scale, h * scale, (hidden ? SDL_WINDOW_HIDDEN : SDL_WINDOW_SHOWN) | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		// Initialize video subsystem

		if (ok)
		{
			// Create OpenGL context
			glContext = SDL_GL_CreateContext(win);

			if (glContext == NULL)
			{
				// Display error message
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				return;
			}
			else
			{
				// Initialize glew
				glewInit();
			}
		}
	}
	void display_glew_Impl::draw_quad(int x, int y) const {
		glTexCoord2f(0.0, 1.0);
		glVertex2f(-1.0f + (float)x / screenWidth * 2, 1.0f - (float)(y + 1) / screenHeight * 2);// 左下角

		glTexCoord2f(1.0, 1.0);
		glVertex2f(-1.0f + (float)(x + 1) / screenWidth * 2, 1.0f - (float)(y + 1) / screenHeight * 2);//右下角

		glTexCoord2f(1.0, 0.0);
		glVertex2f(-1.0f + (float)(x + 1) / screenWidth * 2, 1.0f - (float)y / screenHeight * 2);//右上角

		glTexCoord2f(0.0, 0.0);
		glVertex2f(-1.0f + (float)x / screenWidth * 2, 1.0f - (float)y / screenHeight * 2);//左上角
	}
	void display_glew_Impl::setupResource()
	{
		glGenTextures(1, &rendertarget);
		glBindTexture(GL_TEXTURE_2D, rendertarget);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		/*
		// 创建深度缓冲区
		glGenRenderbuffersEXT(1, &depthbuffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, TEXTURE_WIDTH, TEXTURE_HEIGHT);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
		*/

		// 创建FBO对象
		glGenFramebuffersEXT(1, &fbo);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, rendertarget, 0);
		//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
		{

		}
	}
	void display_glew_Impl::drawVideo(uint8_t* videoBuffer)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_QUADS);
		for (int y = 0; y < screenHeight; y++) {
			for (int x = 0; x < screenWidth; x++) {
				auto scan = screenWidth * y + x;
				if (videoBuffer[scan] > 0)
					draw_quad(x, y);
			}
		}
		glEnd();
		SDL_GL_SwapWindow(win);
		//SDL_Delay(40);
	}
	void display_glew_Impl::drawTexture(uint8_t* videoBuffer)
	{
		glBindTexture(GL_TEXTURE_2D, 0); // 取消绑定，因为如果不取消，渲染到纹理的时候会使用纹理本身

	 // 绑定渲染目标
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_QUADS);
		for (int y = 0; y < screenHeight; y++) {
			for (int x = 0; x < screenWidth; x++) {
				auto scan = screenWidth * y + x;
				if (videoBuffer[scan] > 0)
					draw_quad(x, y);
			}
		}
		glEnd();
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
}

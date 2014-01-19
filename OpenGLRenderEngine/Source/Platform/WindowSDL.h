#pragma once
#include "../TypeDef.h"
#include <SDL/include/SDL.h>

class WindowSDL
{
	public:
	WindowSDL();
	~WindowSDL();

	DataType getHandle();
	SDL_Window* getSDLWindow() { return m_sdlWindow; }

	bool create(const string& title, int x, int y, int width, int height);
	void destroy();

    virtual void onKeyPress(unsigned char nkey);
    virtual void onKeyRelease(unsigned char nkey);
	virtual void onMouseClick(const unsigned char nkey, unsigned int xpos, unsigned int ypos);
	virtual void onMouseRelease(const unsigned char nkey, unsigned int xpos, unsigned int ypos);

	private:
	SDL_Window* m_sdlWindow;
};
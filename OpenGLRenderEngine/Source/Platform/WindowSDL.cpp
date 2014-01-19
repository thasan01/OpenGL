#include "WindowSDL.h"
#include <SDL/include/SDL.h>
#include <SDL/include/SDL_syswm.h>

WindowSDL::WindowSDL() : m_sdlWindow(NULL)
{
}

WindowSDL::~WindowSDL()
{
	destroy();
}

DataType WindowSDL::getHandle()
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version); 
	SDL_GetWindowWMInfo(m_sdlWindow, &info);
	return info.info.win.window;
}

bool WindowSDL::create(const string& title, int x, int y, int width, int height)
{
	destroy();

	m_sdlWindow = SDL_CreateWindow(title.c_str(), x, y, width, height, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	return (m_sdlWindow!=NULL);
}

void WindowSDL::destroy()
{
	if(m_sdlWindow!=NULL)
	{
		SDL_DestroyWindow(m_sdlWindow);
	}
}

void WindowSDL::onKeyPress(unsigned char nkey)
{
	if(nkey==SDLK_ESCAPE)
	{
		SDL_Event event;
		event.type = SDL_QUIT;
		SDL_PushEvent(&event);
	}
}

void WindowSDL::onKeyRelease(unsigned char nkey)
{
}

void WindowSDL::onMouseClick(const unsigned char nkey, unsigned int xpos, unsigned int ypos)
{
}

void WindowSDL::onMouseRelease(const unsigned char nkey, unsigned int xpos, unsigned int ypos)
{
}


#pragma once
#include <SDL\include\SDL.h>
#include <GL\glew.h>
#include <SDL\include\SDL_opengl.h>
#include <GL\glu.h>
#include <GL\gl.h>

class ScreenSDLGL
{
	public:
	ScreenSDLGL(SDL_Window& window);
	~ScreenSDLGL();

	int getMajorVersion() const { return m_majorVersion; }
	int getMinorVersion() const { return m_minorVersion; }

	virtual void onRender() {}

	void render(SDL_Window& window);
	private:

	SDL_GLContext m_context;
	int m_majorVersion;
	int m_minorVersion;
};
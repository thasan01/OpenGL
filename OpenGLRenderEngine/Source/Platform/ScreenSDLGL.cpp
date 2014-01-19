#include "ScreenSDLGL.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/GLU.h>


ScreenSDLGL::ScreenSDLGL(SDL_Window& window) :m_context(SDL_GL_CreateContext(&window))
{
	glGetIntegerv(GL_MAJOR_VERSION, &m_majorVersion); 
	glGetIntegerv(GL_MINOR_VERSION, &m_minorVersion);

	GLenum error = glewInit(); 
	if (error != GLEW_OK) 
	{
		//return false;
	}
}

ScreenSDLGL::~ScreenSDLGL()
{
	SDL_GL_DeleteContext(m_context);  
}

void ScreenSDLGL::render(SDL_Window& window)
{
	onRender();
	SDL_GL_SwapWindow(&window);
}

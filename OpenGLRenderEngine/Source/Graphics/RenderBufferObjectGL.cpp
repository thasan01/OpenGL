#include "RenderBufferObjectGL.h"

  RenderBufferObjectGL::RenderBufferObjectGL(unsigned int width, unsigned int height, bool isDepthBuffer)
	  :m_isDepth(isDepthBuffer)
  {
		glGenTextures(1,&m_rboID);
		glBindTexture(GL_TEXTURE_2D,m_rboID);
		glTexImage2D(GL_TEXTURE_2D,0,(!isDepthBuffer ? GL_RGBA8 : GL_DEPTH_COMPONENT),width,height,0,isDepthBuffer ? GL_DEPTH_COMPONENT : GL_RGBA,GL_FLOAT,NULL);

		int filterType = isDepthBuffer ? GL_LINEAR : GL_NEAREST;
		int wrapType = isDepthBuffer ? GL_CLAMP : GL_CLAMP_TO_EDGE;

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filterType);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filterType);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, wrapType);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, wrapType);

		glBindTexture(GL_TEXTURE_2D,0);

//		if(error!=GL_NO_ERROR) throw exception("error");
  }
  
  RenderBufferObjectGL::~RenderBufferObjectGL()
  {
	glDeleteTextures(1, &m_rboID);
  }

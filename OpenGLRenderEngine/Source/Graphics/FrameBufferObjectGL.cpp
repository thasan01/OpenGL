#include "FrameBufferObjectGL.h"

	FrameBufferObjectGL::FrameBufferObjectGL()
	{
		glGenFramebuffers(1, &m_fboID);
	}
	
	FrameBufferObjectGL::~FrameBufferObjectGL()
	{
	  unbind();
	  glDeleteFramebuffers(1, &m_fboID);
	}

	void FrameBufferObjectGL::bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
	}
	
	void FrameBufferObjectGL::unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBufferObjectGL::attach(const RenderBufferObjectGL& buffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
		GLenum attachment = (buffer.isDepthBuffer() ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, buffer.getBufferID(),0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void FrameBufferObjectGL::swap(const RenderBufferObjectGL& buffer) //assumes fbo is already bound
	{
		GLenum attachment = (buffer.isDepthBuffer() ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, buffer.getBufferID(),0);
	}

#include "ShadowMapGL.h"
#include <exception>

ShadowMapGL::ShadowMapGL(unsigned int width, unsigned int height)
	: m_fbo(), m_dbo(width,height,true), m_winWidth(width), m_winHeight(height)
{
	m_fbo.bind();

	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,m_dbo.getBufferID(),0);

	int status=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status!=GL_FRAMEBUFFER_COMPLETE)
		throw exception("Error creating shadow map frame buffer.");

	m_fbo.unbind();
}

ShadowMapGL::~ShadowMapGL()
{
}


void ShadowMapGL::beginFirstPass()
{
	m_fbo.bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,0,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, m_dbo.getBufferID(),0);

	glViewport(0,0,m_winWidth,m_winHeight);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMapGL::endFirstPass(unsigned int width, unsigned int height)
{
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0,0,width,height);
}

void ShadowMapGL::beginSecondPass(unsigned int depthTextureIndex)
{
	glActiveTexture(depthTextureIndex);
	glBindTexture(GL_TEXTURE_2D,m_dbo.getBufferID());
}

void ShadowMapGL::endSecondPass()
{
}



#pragma once
#include <GL\glew.h>
#include <GL\glu.h>
#include <GL\gl.h>
#include "..\TypeDef.h"
class RenderBufferObjectGL
{
	public:
	RenderBufferObjectGL(unsigned int width, unsigned int height, bool isDepthBuffer=false);
	virtual ~RenderBufferObjectGL();

	inline unsigned int getBufferID() const { return m_rboID; }
	inline bool isDepthBuffer() const { return m_isDepth; }

	protected:
	bool			m_isDepth;
	unsigned int	m_rboID;
};

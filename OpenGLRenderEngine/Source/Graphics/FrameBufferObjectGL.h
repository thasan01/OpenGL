#pragma once
#include "RenderBufferObjectGL.h"

class FrameBufferObjectGL
{
	public:
	FrameBufferObjectGL();
	virtual ~FrameBufferObjectGL();

	void bind() const;
	void unbind() const;

	void attach(const RenderBufferObjectGL& buffer);
	void swap(const RenderBufferObjectGL& buffer); //assumes fbo is already bound

	protected:
	unsigned int m_fboID;
};

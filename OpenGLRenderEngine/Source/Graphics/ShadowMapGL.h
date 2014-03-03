#pragma once

#include <GL/glew.h>
#include <GL/wglew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "FrameBufferObjectGL.h"
#include "RenderBufferObjectGL.h"

class ShadowMapGL
{
  public:
  ShadowMapGL(unsigned int width, unsigned int height);
  ~ShadowMapGL();

  //1st pass renders scene to depth buffer from light's point of view
  void beginFirstPass();
  void endFirstPass(unsigned int width, unsigned int height);

  //2nd pass renders to screen from camera's point of view
  void beginSecondPass(unsigned int depthTextureIndex);
  void endSecondPass();

  private:

  FrameBufferObjectGL  m_fbo;
  RenderBufferObjectGL m_dbo;

  unsigned int m_winWidth;
  unsigned int m_winHeight;
};
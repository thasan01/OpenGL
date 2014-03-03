#include "../Globals.h"

MaterialGL::MaterialGL(	const string& materialName, const float ambient[], const float diffuse[], const float specular[], float shininess, 
							const shared_ptr<TextureGL>& texture )
		: m_materialName(materialName), m_ptrTexture(texture)
	{
		for(int i=0; i<4; i++)
		{
			m_ambient.m_c[i] = ambient[i];
			m_diffuse.m_c[i] = diffuse[i];
			m_specular.m_c[i] = specular[i];
		}
		m_shininess = shininess;
	}

	MaterialGL::~MaterialGL()
	{
		if(m_ptrTexture)
			graphics.releaseTexture(m_ptrTexture);
	}


  void MaterialGL::bind(const SceneShaderInfo& shaderInfo, shared_ptr<ShaderProgramGL>& shaderProgram)
  {
//*
	if(m_ptrTexture)
		m_ptrTexture->bind();

	shaderProgram->setVariableVector3f(shaderInfo.m_materialAmbientLocation, m_ambient.m_c);
	shaderProgram->setVariableVector3f(shaderInfo.m_materialDiffuseLocation, m_diffuse.m_c);
	shaderProgram->setVariableVector3f(shaderInfo.m_materialSpecularLocation, m_specular.m_c);
	shaderProgram->setVariableFloat(shaderInfo.m_materialShininessLocation, m_shininess);
//*/
  }

  void MaterialGL::unbind()
  {
//	  if(m_ptrTexture)
//		  m_ptrTexture->unbind();
  }


  const float* MaterialGL::getAmbient() const
  {
	return m_ambient.m_c;
  }


  const float* MaterialGL::getDiffuse() const
  {
	return m_diffuse.m_c;
  }


  const float* MaterialGL::getSpecular() const
  {
	  return m_specular.m_c;
  }


  float MaterialGL::getShininess() const
  {
	return m_shininess;
  }

  void MaterialGL::setAmbient(const float ambient[])
  {
	for(int i=0; i<4; i++)
		m_ambient.m_c[i] = ambient[i];
  }


  void MaterialGL::setDiffuse(const float diffuse[])
  {
	for(int i=0; i<4; i++)
		m_diffuse.m_c[i] = diffuse[i];
  }


  void MaterialGL::setSpecular(const float specular[])
  {
	for(int i=0; i<4; i++)
		m_specular.m_c[i] = specular[i];
  }


  void MaterialGL::setShininess(float shininess)
  {
	m_shininess = shininess;
  }


  const shared_ptr<TextureGL>& MaterialGL::getTexture() const
  {
	  return m_ptrTexture;
  }

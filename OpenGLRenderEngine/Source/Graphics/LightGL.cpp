#include "LightGL.h"



	LightGL::LightGL(const float ambient[], const float diffuse[], const float specular[], const float position[])
	{
		int i;
		for(i=0; i<4; i++)
		{
			m_ambient.m_c[i] = ambient[i];
			m_diffuse.m_c[i] = diffuse[i];
			m_specular.m_c[i] = specular[i];
		}

		for(i=0; i<3; i++)
		{
			m_position.m_v[i] = position[i];
		}
	}

	LightGL::~LightGL()
	{
	}

	const float* LightGL::getAmbient() const
	{
		return m_ambient.m_c;		
	}
	
	const float* LightGL::getDiffuse() const
	{
		return m_diffuse.m_c;		
	}
	
	const float* LightGL::getSpecular() const
	{
		return m_specular.m_c;		
	}
	
	const float* LightGL::getPosition() const
	{
		return m_position.m_v;		
	}

	
	void LightGL::setAmbient(const float ambient[])
	{
		for(int i=0; i<4; i++)
			m_ambient.m_c[i] = ambient[i];
	}
	
	void LightGL::setDiffuse(const float diffuse[])
	{
		for(int i=0; i<4; i++)
			m_diffuse.m_c[i] = diffuse[i];
	}
	
	void LightGL::setSpecular(const float specular[])
	{
		for(int i=0; i<4; i++)
			m_specular.m_c[i] = specular[i];
	}
	
	void LightGL::setPosition(const float position[])
	{
		for(int i=0; i<3; i++)
			m_position.m_v[i] = position[i];
	}


	void LightGL::bind(const SceneShaderInfo& sceneInfo, ShaderProgramGL& shaderProgram)
	{
		shaderProgram.setVariableVector3f(sceneInfo.m_lightAmbientLocation, m_ambient.m_c);
		shaderProgram.setVariableVector3f(sceneInfo.m_lightDiffuseLocation, m_diffuse.m_c);
		shaderProgram.setVariableVector3f(sceneInfo.m_lightSpecularLocation, m_specular.m_c);
		shaderProgram.setVariableVector3f(sceneInfo.m_lightPositionLocation, m_position.m_v);
	}
	
	void LightGL::unbind(const SceneShaderInfo& sceneInfo, ShaderProgramGL& shaderProgram)
	{
	}


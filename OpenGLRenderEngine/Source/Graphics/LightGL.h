#pragma once
#include "../File/MeshData.h"
#include "ShaderGL.h"

	class LightGL
	{
		public:

		LightGL(const float ambient[], const float diffuse[], const float specular[], const float position[]);
		virtual ~LightGL();

		const float* getAmbient() const;
		const float* getDiffuse() const;
		const float* getSpecular() const;
		const float* getPosition() const;

		void setAmbient(const float ambient[]);
		void setDiffuse(const float diffuse[]);
		void setSpecular(const float specular[]);
		void setPosition(const float position[]);

		void bind(const SceneShaderInfo& sceneInfo, ShaderProgramGL& shaderProgram);
		void unbind(const SceneShaderInfo& sceneInfo, ShaderProgramGL& shaderProgram);		

		protected:
		Color4  m_ambient;
		Color4  m_diffuse;
		Color4  m_specular;
		Vertex3 m_position;
	};


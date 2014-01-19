#pragma once
#include "../TypeDef.h"

#include <GL/glew.h>
#include <GL/wglew.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "../File/MeshData.h"
#include "ShaderGL.h"
#include "TextureGL.h"

	class  MaterialGL
	{
		public:

		MaterialGL(	const string& materialName, const float ambient[], const float diffuse[], const float specular[], float shininess, 
					const shared_ptr<TextureGL>& texture);

		virtual ~MaterialGL();

		const string& getName() const { return m_materialName; }

		void bind(const SceneShaderInfo& shaderInfo, shared_ptr<ShaderProgramGL>& shaderProgram);
		void unbind();

		const float* getAmbient() const;
		const float* getDiffuse() const;
		const float* getSpecular() const;
		float getShininess() const;

		void setAmbient(const float ambient[]);
		void setDiffuse(const float diffuse[]);
		void setSpecular(const float specular[]);
		void setShininess(float shininess);

		const shared_ptr<TextureGL>& getTexture() const;

		protected:
		string	m_materialName;
		Color4 m_ambient;
		Color4 m_diffuse;
		Color4 m_specular;
		float m_shininess;

		shared_ptr<TextureGL> m_ptrTexture;
		friend class GraphicsGL;
	};

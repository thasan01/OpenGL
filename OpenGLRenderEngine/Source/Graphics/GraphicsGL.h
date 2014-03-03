#pragma once
#include "../TypeDef.h"
#include "../ResourceLibrary.h"
#include "TextureGL.h"
#include "ShaderGL.h"
#include "MaterialGL.h"
#include "MeshGL.h"
#include "LightGL.h"
#include "FrameBufferObjectGL.h"
#include "ShadowMapGL.h"

class GraphicsGL
{
	public:
	shared_ptr<TextureGL> createTexture(const string& filename);
	shared_ptr<ShaderGL> createShader(ShaderGL::Type type, const string& filename);
	shared_ptr<ShaderProgramGL> createShaderProgram(const string& shaderProgramName,
		const vector<string>& attributeNameList,
		const vector<shared_ptr<ShaderGL>>& vertexShaderList,
		const vector<shared_ptr<ShaderGL>>& fragmentShaderList);

	shared_ptr<MaterialGL> createMaterial(const string& materialName, const float ambient[], const float diffuse[], const float specular[], float shininess, const shared_ptr<TextureGL>& texture);

	shared_ptr<MeshGL> createMesh(const MeshData& meshData, const vector<MaterialData>& materialDataList);
	shared_ptr<MeshGL> createMesh(const string& meshName, const MeshFileData& meshFileData);


	void releaseShader(shared_ptr<ShaderGL>& ptr);
	void releaseShaderProgram(shared_ptr<ShaderProgramGL>& ptr);
	void releaseTexture(shared_ptr<TextureGL>& ptr);
	void releaseMaterial(shared_ptr<MaterialGL>& ptr);
	void releaseMesh(shared_ptr<MeshGL>& ptr);

	private:
	ResourceLibrary<string,TextureGL> m_textureResLib;
	ResourceLibrary<string,MaterialGL> m_materialResLib;
	ResourceLibrary<string,ShaderGL> m_shaderResLib;
	ResourceLibrary<string,ShaderProgramGL> m_shaderProgramResLib;
	ResourceLibrary<string, MeshGL> m_meshResLib;

};
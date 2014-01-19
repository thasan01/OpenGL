#include "GraphicsGL.h"

shared_ptr<TextureGL> GraphicsGL::createTexture(const string& filename)
{
	shared_ptr<TextureGL> ptr = m_textureResLib.get(filename);
	if(!ptr)
	{
		ptr.reset(new TextureGL(filename));
		m_textureResLib.add(filename, ptr);
	}
	return ptr;
}

shared_ptr<ShaderGL> GraphicsGL::createShader(ShaderGL::Type type, const string& filename)
{
	string source;
	shared_ptr<ShaderGL> ptr;
	if(ShaderGL::loadShaderFromFile(filename, source))
	{
		unsigned int shaderID=0;
		if(type == ShaderGL::VERTEX_SHADER)
			shaderID =  glCreateShader( GL_VERTEX_SHADER );
		else if(type == ShaderGL::FRAGMENT_SHADER)
			shaderID =  glCreateShader( GL_FRAGMENT_SHADER );

		const char* ptrString = source.data();
		glShaderSource(shaderID, 1, &ptrString, 0);
		glCompileShader(shaderID);

		ptr.reset(new ShaderGL);
		ptr->m_filename = filename;
		ptr->m_shaderID = shaderID;
	}
	return ptr; 
}

shared_ptr<ShaderProgramGL> GraphicsGL::createShaderProgram(const string& shaderProgramName, const vector<string>& attributeNameList, 
	const vector<shared_ptr<ShaderGL>>& vertexShaderList, const vector<shared_ptr<ShaderGL>>& fragmentShaderList)
{
	shared_ptr<ShaderProgramGL> ptr = m_shaderProgramResLib.get(shaderProgramName);
	if(!ptr)
	{
		ptr.reset( new ShaderProgramGL(shaderProgramName, attributeNameList, vertexShaderList, fragmentShaderList) );
		m_shaderProgramResLib.add(shaderProgramName, ptr);
	}
	return ptr;
}

shared_ptr< MaterialGL > GraphicsGL::createMaterial(const string& materialName, const float ambient[], const float diffuse[], const float specular[], float shininess, 
	const shared_ptr<TextureGL>& texture)
{
	shared_ptr<MaterialGL> ptr = m_materialResLib.get(materialName);
	if(!ptr)
	{
		ptr.reset(new MaterialGL(materialName, ambient, diffuse, specular, shininess, texture ));
		m_materialResLib.add(materialName, ptr);
	}
	return ptr;
}


shared_ptr<MeshGL> GraphicsGL::createMesh(const MeshData& meshData, const vector<MaterialData>& materialDataList)
{
	return shared_ptr<MeshGL>( new MeshGL( meshData, materialDataList) );
}

shared_ptr<MeshGL> GraphicsGL::createMesh(const string& meshName, const MeshFileData& meshFileData)
{
	return shared_ptr<MeshGL>( new MeshGL(meshName, meshFileData) );
}


void GraphicsGL::releaseTexture(shared_ptr<TextureGL>& ptr)
{
	m_textureResLib.remove(ptr->getFileName());
	ptr.reset();
}

void GraphicsGL::releaseMaterial(shared_ptr<MaterialGL>& ptr)
{
	m_materialResLib.remove(ptr->getName());
	ptr.reset();
}


void GraphicsGL::releaseShader(shared_ptr<ShaderGL>& ptr)
{
	if(m_shaderResLib.remove(ptr->m_filename))
		glDeleteShader(ptr->m_shaderID);
	ptr.reset();
}

void GraphicsGL::releaseShaderProgram(shared_ptr<ShaderProgramGL>& ptr)
{
	string key = ptr->getName();
	m_shaderProgramResLib.remove(ptr->getName());
	ptr.reset();
}

void GraphicsGL::releaseMesh(shared_ptr<MeshGL>& ptr)
{
	m_meshResLib.remove(ptr->getName());
	ptr.reset();
}


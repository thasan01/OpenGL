#pragma once
#include "../TypeDef.h"

#include "../File/MeshData.h"

#include <GL/glew.h>
#include <GL/wglew.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "TextureGL.h"
#include "ShaderGL.h"
#include "MaterialGL.h"

	class VertexBufferObject
	{
		public:
		enum Type{VERTEX_BUFFER=0, NORMAL_BUFFER=1, TEXTCOORD_BUFFER=2, COLOR_BUFFER=3};

		VertexBufferObject(Type bufferType, unsigned int bufferArraySize, const DataType bufferArray);
		virtual ~VertexBufferObject();

		Type getType() const { return m_type; }
		unsigned int getBufferID() const { return m_bufferID; }
		unsigned int getArraySize() const { return m_numElements; }
		unsigned int getByteSize() const { return m_byteSize; }

		protected:
		Type			m_type;
		unsigned int	m_bufferID;
		unsigned int	m_numElements;
		unsigned int	m_byteSize;
	};


	class IndexBufferObject
	{
		public:
		IndexBufferObject(unsigned int maxTriIndices, const TriIndex triIndex[]);
		virtual ~IndexBufferObject();

		unsigned int getIndexID() const { return m_iboID; }
		unsigned int getArraySize() const { return m_numElements; }
		unsigned int getByteSize() const { return m_byteSize; }

		protected:
		unsigned int m_iboID;
		unsigned int m_numElements;
		unsigned int m_byteSize;

		unsigned int m_maxIndex;
		unsigned int m_minIndex;
	};


	class VertexArrayObject
	{
		public:
		#define	MAX_VBO_ATTRIBUTES	4

		VertexArrayObject(	const vector<shared_ptr<VertexBufferObject>>& ptrVBOList, 
							const shared_ptr<IndexBufferObject>& ptrIBO);
		virtual ~VertexArrayObject();

		virtual void render(unsigned int renderType = GL_TRIANGLES);

		inline VertexBufferObject& getVBO(unsigned int n) { return *m_ptrVBO[n]; }
		inline IndexBufferObject& getIBO() { return *m_ptrIBO; }

		protected:
		unsigned int m_vaoID;
		shared_ptr<VertexBufferObject> m_ptrVBO[MAX_VBO_ATTRIBUTES];
		shared_ptr<IndexBufferObject>	m_ptrIBO;
	};


	class MeshAttributeGL : public AttributeHandle
	{
	  public:
	  MeshAttributeGL(const unsigned int& vboID, const unsigned int& arraySize, unsigned int byteSize, GLenum targetBufferType);
	  ~MeshAttributeGL();

	  unsigned int getArraySize() const { return m_arraySize; }

	  private:
	  GLenum m_target;
	  const unsigned int m_vboID;
	  const unsigned int m_arraySize;
	};

	class MeshAttributeListGL : public AttributeListHandle
	{
		public:
		MeshAttributeListGL(const vector<unsigned int>& vboIDList, 
							const vector<unsigned int>& arraySizeList, 
							const vector<unsigned int>& byteSizeList, 
							GLenum targetBufferType);
		~MeshAttributeListGL();

		private:
		GLenum m_target;
		vector<unsigned int> m_vboIDList;
	};

	class MeshGL
	{
		public:
		MeshGL(const MeshData& meshData, const vector<MaterialData>& materialDataList);
		MeshGL(const string& meshName, const MeshFileData& meshFileData);
		virtual ~MeshGL();

		const string& getName() const { return m_name; }

		MeshAttributeGL getVertexAttribute(unsigned int bufferType);
		MeshAttributeListGL getVertexAttributeList();
		MeshAttributeGL getTriIndexAttribute(unsigned int n);
		MeshAttributeListGL getTriIndexAttributeList();

		const shared_ptr<MaterialGL>& getMaterial(unsigned int n) const;
		void render(const SceneShaderInfo& shaderInfo, shared_ptr<ShaderProgramGL>& shaderProgram) const;

		protected:
		const string					m_name;
		vector<unsigned int>			m_matIndexList;
		vector<VertexArrayObject*>		m_vaoList;
		vector<shared_ptr<MaterialGL>>	m_materialList; 
	};

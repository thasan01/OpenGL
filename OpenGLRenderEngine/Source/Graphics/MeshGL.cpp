#include "MeshGL.h"
#include "../Globals.h"

  //===================================
  // VertexBufferObject
  //===================================

	VertexBufferObject::VertexBufferObject(Type bufferType, unsigned int bufferArraySize, const DataType bufferArray)
		:m_type(bufferType), m_numElements(bufferArraySize)
	{
		glGenBuffers(1, &m_bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);

		switch(bufferType)
		{
			case VERTEX_BUFFER:
			{
				const Vertex3* buffer = (const  Vertex3*) bufferArray; 
				m_byteSize = bufferArraySize*3*sizeof(float);
				glBufferData(GL_ARRAY_BUFFER, m_byteSize, &buffer[0].m_x, GL_STATIC_DRAW);
				return;
			}
			case NORMAL_BUFFER:
			{
				const Vertex3* buffer = (const Vertex3*) bufferArray; 
				m_byteSize = bufferArraySize*3*sizeof(float);
				glBufferData(GL_ARRAY_BUFFER, m_byteSize, &buffer[0].m_x, GL_STATIC_DRAW);
				return;
			}
			case TEXTCOORD_BUFFER:
			{
				const TextCoord* buffer = (const TextCoord*) bufferArray; 
				m_byteSize = bufferArraySize*2*sizeof(float);
				glBufferData(GL_ARRAY_BUFFER, m_byteSize, &buffer[0].m_u, GL_STATIC_DRAW);
				return;
			}
			case COLOR_BUFFER:
			{
				const Color4* buffer = (const Color4*) bufferArray; 
				m_byteSize = bufferArraySize*4*sizeof(float);
				glBufferData(GL_ARRAY_BUFFER, m_byteSize, &buffer[0].m_r, GL_STATIC_DRAW);
				return;
			}
			default:
				return;		
		}

	}
	
	VertexBufferObject::~VertexBufferObject()
	{
		glDeleteBuffers(1, &m_bufferID);
	}


  //===================================
  // IndexBufferObject
  //===================================

	IndexBufferObject::IndexBufferObject(unsigned int maxTriIndices, const TriIndex triIndex[])
		:m_numElements(maxTriIndices * 3), m_byteSize(m_numElements*sizeof(unsigned short))
	{
		glGenBuffers(1, &m_iboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_byteSize, &triIndex[0].m_a, GL_STATIC_DRAW);
	}

	IndexBufferObject::~IndexBufferObject()
	{
		glDeleteBuffers(1, &m_iboID);
	}


  //===================================
  // VertexArrayObject
  //===================================

	VertexArrayObject::VertexArrayObject(const vector<shared_ptr<VertexBufferObject>>& ptrVBOList, 
										 const shared_ptr<IndexBufferObject>& ptrIBO)
		:m_ptrIBO(ptrIBO)
	{
		glGenVertexArrays(1, &m_vaoID);
		glBindVertexArray(m_vaoID);


		for(unsigned int i=0, j=ptrVBOList.size(); i<j; i++)
		{
			GLint  attrSize = 3;
			GLboolean isNormalized = GL_FALSE;

			VertexBufferObject::Type bufferType = ptrVBOList[i]->getType();
			if(bufferType == VertexBufferObject::TEXTCOORD_BUFFER)
			{
				attrSize = 2;
			}
			else if(bufferType == VertexBufferObject::NORMAL_BUFFER)
			{
				isNormalized = GL_TRUE;
			}
			else if(bufferType == VertexBufferObject::COLOR_BUFFER)
			{
				attrSize = 4;
				isNormalized = GL_TRUE;
			}

			m_ptrVBO[unsigned int(bufferType)] = ptrVBOList[i];
			glBindBuffer(GL_ARRAY_BUFFER, ptrVBOList[i]->getBufferID() );
			glVertexAttribPointer( GLuint(bufferType), attrSize, GL_FLOAT, isNormalized, 0, (void*)0 );
		}

		for(int i=0; i<MAX_VBO_ATTRIBUTES; i++)
			(m_ptrVBO[i] ? glEnableVertexAttribArray(i) : glDisableVertexAttribArray(i));


		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ptrIBO->getIndexID());
		glVertexAttribPointer( 5, 3, GL_UNSIGNED_SHORT, GL_FALSE, 0, (void*)0 );

		glBindVertexArray(0);

	}

	VertexArrayObject::~VertexArrayObject()
	{
	}


	void VertexArrayObject::render(unsigned int renderType)
	{
		glBindVertexArray(m_vaoID);
		glDrawElements(renderType, m_ptrIBO->getArraySize(), GL_UNSIGNED_SHORT, (void*)0 );
		glBindVertexArray(0);
	}


  //===================================
  // MeshAttributeGL
  //===================================
	MeshAttributeGL::MeshAttributeGL(const unsigned int& vboID, const unsigned int& arraySize, unsigned int byteSize, GLenum targetBufferType)
		: AttributeHandle(NULL), m_vboID(vboID), m_arraySize(arraySize), m_target(targetBufferType)
	{ //m_target = GL_ARRAY_BUFFER | 
		glBindBuffer(m_target, m_vboID);
		m_data = (void*)glMapBufferRange(m_target, 0, byteSize, GL_MAP_READ_BIT);
		glBindBuffer(m_target, 0);
	}

	MeshAttributeGL::~MeshAttributeGL()
	{
		glBindBuffer(m_target, m_vboID);
		glUnmapBuffer(m_target);
		glBindBuffer(m_target, 0);
	}


  //===================================
  // MeshAttributeListGL
  //===================================

	MeshAttributeListGL::MeshAttributeListGL(const vector<unsigned int>& vboIDList, 
							const vector<unsigned int>& arraySizeList, 
							const vector<unsigned int>& byteSizeList, 
							GLenum targetBufferType)
		: AttributeListHandle(vector<void*>(), arraySizeList), m_target(targetBufferType), m_vboIDList(vboIDList)
	{

		m_data.resize(vboIDList.size());
		for(unsigned int i=0, j=vboIDList.size(); i<j; i++)
		{
			glBindBuffer(m_target, vboIDList[i]);
			m_data[i]=(void*)glMapBufferRange(m_target, 0, byteSizeList[i], GL_MAP_READ_BIT);
		}
	}
	
	MeshAttributeListGL::~MeshAttributeListGL()
	{
		for(unsigned int i=0, j=m_vboIDList.size(); i<j; i++)
		{
			glBindBuffer(m_target, m_vboIDList[i]);
			glUnmapBuffer(m_target);
		}
		glBindBuffer(m_target, 0);
	}


  //===================================
  // MeshGL
  //===================================

	MeshGL::MeshGL(const MeshData& meshData, const vector<MaterialData>& materialDataList)
		: m_name(meshData.m_name)
	{
		unsigned int maxVertices = meshData.m_vertex.size();
		unsigned int maxUsedMaterials = meshData.m_matRange.size(); //# of materials actually used by the mesh
		GraphicsGL& gfx = graphics;

		//load vertex data
		vector<shared_ptr<VertexBufferObject>> ptrVBOList;
		ptrVBOList.push_back(shared_ptr<VertexBufferObject>(new VertexBufferObject(VertexBufferObject::VERTEX_BUFFER, meshData.m_vertex.size(), (const DataType)&meshData.m_vertex[0])));
		ptrVBOList.push_back(shared_ptr<VertexBufferObject>(new VertexBufferObject(VertexBufferObject::NORMAL_BUFFER, meshData.m_normal.size(), (const DataType)&meshData.m_normal[0])));
		ptrVBOList.push_back(shared_ptr<VertexBufferObject>(new VertexBufferObject(VertexBufferObject::TEXTCOORD_BUFFER, meshData.m_textCoord.size(), (const DataType)&meshData.m_textCoord[0])));

		//load triangle face data
		m_vaoList.reserve(maxUsedMaterials);
		unsigned int pos=0; 
		for(unsigned int i=0; i<maxUsedMaterials; i++)
		{
			shared_ptr<IndexBufferObject> ptrIBO( new IndexBufferObject(meshData.m_matRange[i], &meshData.m_triIndex[pos]) );	
			pos += meshData.m_matRange[i];

			//create a VertexArrayObject for each material
			m_vaoList.push_back( new VertexArrayObject(ptrVBOList, ptrIBO) );
		}

		//create the materials from materialDataList
		m_materialList.reserve(maxUsedMaterials); 
		m_matIndexList.reserve(maxUsedMaterials);
		for(unsigned int i=0; i<maxUsedMaterials; i++)
		{
			const MaterialData& materialData = materialDataList[meshData.m_matIndexID[i]];
			shared_ptr<TextureGL> ptrTexture;

			if(!materialData.m_textureList.empty())
			{
				const string& filename = materialData.m_textureList[0].m_filename;
				ptrTexture = gfx.createTexture(filename);
			}

			shared_ptr<MaterialGL> materialPtr = gfx.createMaterial(materialData.m_name, materialData.m_ambient.m_c, materialData.m_diffuse.m_c, 
													materialData.m_specular.m_c, materialData.m_shininess, ptrTexture);

			if(!materialPtr)
				throw exception("mesh material is null.");

			m_materialList.push_back( materialPtr );
			m_matIndexList.push_back( i );
		}
	}
	
	MeshGL::MeshGL(const string& meshName, const MeshFileData& meshFileData)
		: m_name(meshName)
	{
		unsigned int i,j;
		GraphicsGL& gfx = graphics;
		unsigned int materialCount = meshFileData.m_materialData.size();

		unsigned int reserveVertexSize = 0;
		unsigned int maxSubMeshes = meshFileData.m_meshData.size();
		for(i=0; i<maxSubMeshes; i++)
		{
			reserveVertexSize += meshFileData.m_meshData[i].m_vertex.size();
		}

		vector<Vertex3> mergedVertex;
		vector<Vertex3> mergedNormal;
		vector<TextCoord> mergedTextCoord;
		vector<vector<TriIndex>> mergedTriIndex(materialCount);

		mergedVertex.reserve(reserveVertexSize);
		mergedNormal.reserve(reserveVertexSize);
		mergedTextCoord.reserve(reserveVertexSize);

		for(i=0; i<maxSubMeshes; i++)
		{
			unsigned int offset = mergedVertex.size();
			for(j=0; j<meshFileData.m_meshData[i].m_vertex.size(); j++)
			{
				mergedVertex.push_back( meshFileData.m_meshData[i].m_vertex[j] );
				mergedNormal.push_back( meshFileData.m_meshData[i].m_normal[j] );
				mergedTextCoord.push_back( meshFileData.m_meshData[i].m_textCoord[j] );
			}

			unsigned int pos=0;
			for(j=0; j<meshFileData.m_meshData[i].m_matRange.size(); j++)
			{
				unsigned int nMat = meshFileData.m_meshData[i].m_matIndexID[j];
				for(unsigned int k=0; k<meshFileData.m_meshData[i].m_matRange[j]; k++)
				{
					TriIndex temp;
					temp.m_a = meshFileData.m_meshData[i].m_triIndex[ k + pos ].m_a + offset;
					temp.m_b = meshFileData.m_meshData[i].m_triIndex[ k + pos ].m_b + offset;
					temp.m_c = meshFileData.m_meshData[i].m_triIndex[ k + pos ].m_c + offset;
					mergedTriIndex[nMat].push_back(temp);
				}
				pos+=meshFileData.m_meshData[i].m_matRange[j];				
			}


		}

		//load vertex data
		vector<shared_ptr<VertexBufferObject>> ptrVBOList;
		ptrVBOList.push_back(shared_ptr<VertexBufferObject>(new VertexBufferObject(VertexBufferObject::VERTEX_BUFFER, mergedVertex.size(), (const DataType)&mergedVertex[0])));
		ptrVBOList.push_back(shared_ptr<VertexBufferObject>(new VertexBufferObject(VertexBufferObject::NORMAL_BUFFER, mergedNormal.size(), (const DataType)&mergedNormal[0])));
		ptrVBOList.push_back(shared_ptr<VertexBufferObject>(new VertexBufferObject(VertexBufferObject::TEXTCOORD_BUFFER, mergedTextCoord.size(), (const DataType)&mergedTextCoord[0])));

		//load triangle face data
		m_vaoList.reserve(materialCount);
		for(unsigned int i=0; i<materialCount; i++)
		{
			if(!mergedTriIndex[i].empty())
			{
				const MaterialData& materialData = meshFileData.m_materialData[i];
				shared_ptr<TextureGL> ptrTexture;

				if(!materialData.m_textureList.empty())
				{
					const string& filename = materialData.m_textureList[0].m_filename;
					ptrTexture = gfx.createTexture(filename);
				}

				shared_ptr<MaterialGL> materialPtr = gfx.createMaterial(materialData.m_name, materialData.m_ambient.m_c, materialData.m_diffuse.m_c, 
															 materialData.m_specular.m_c, materialData.m_shininess, ptrTexture);
				m_materialList.push_back( materialPtr );

				shared_ptr<IndexBufferObject> ptrIBO( new IndexBufferObject(mergedTriIndex[i].size(), &mergedTriIndex[i][0]) );	
				//create a VertexArrayObject for each material
				m_vaoList.push_back( new VertexArrayObject(ptrVBOList, ptrIBO) );
			}
		}


	}


	MeshGL::~MeshGL()
	{
		for(unsigned int i=0; i<m_vaoList.size(); i++)
			delete m_vaoList[i];

		for(unsigned int i=0; i<m_materialList.size(); i++)
			graphics.releaseMaterial(m_materialList[i]);
	}

	const shared_ptr<MaterialGL>& MeshGL::getMaterial(unsigned int n) const
	{
		return m_materialList[n];
	}
		
	MeshAttributeGL MeshGL::getVertexAttribute(unsigned int bufferType)
	{
		VertexBufferObject& vbo = m_vaoList[0]->getVBO( bufferType );
		return MeshAttributeGL(vbo.getBufferID(), vbo.getArraySize(), vbo.getByteSize(), GL_ARRAY_BUFFER);
	}

	MeshAttributeListGL MeshGL::getVertexAttributeList()
	{
		vector<unsigned int> vboIDList(3);
		vector<unsigned int> arraySizeList(3);
		vector<unsigned int> byteSizeList(3);
							
		for(unsigned int i=0; i<3; i++)
		{
			VertexBufferObject& vbo = m_vaoList[0]->getVBO(i);
			vboIDList[i] = vbo.getBufferID();
			arraySizeList[i] = vbo.getArraySize();
			byteSizeList[i] = vbo.getByteSize();			
		}

		return MeshAttributeListGL(vboIDList, arraySizeList, byteSizeList, GL_ARRAY_BUFFER);
	}


	MeshAttributeGL MeshGL::getTriIndexAttribute(unsigned int n)
	{
		IndexBufferObject& ibo = m_vaoList[n]->getIBO();
		return MeshAttributeGL(ibo.getIndexID(), ibo.getArraySize()/3, ibo.getByteSize(), GL_ELEMENT_ARRAY_BUFFER);
	}

	MeshAttributeListGL MeshGL::getTriIndexAttributeList()
	{
		unsigned int vaoCount = m_vaoList.size();
		vector<unsigned int> iboIDList(vaoCount);
		vector<unsigned int> arraySizeList(vaoCount);
		vector<unsigned int> byteSizeList(vaoCount);
		
		
		for(unsigned int i=0; i<vaoCount; i++)
		{
			auto& ibo = m_vaoList[i]->getIBO();
			iboIDList[i] = ibo.getIndexID();
			arraySizeList[i] = ibo.getArraySize()/3;
			byteSizeList[i] = ibo.getByteSize();			
		}

		return MeshAttributeListGL(iboIDList, arraySizeList, byteSizeList, GL_ELEMENT_ARRAY_BUFFER);
	}

	void MeshGL::render(const SceneShaderInfo& shaderInfo, shared_ptr<ShaderProgramGL>& shaderProgram) const
	{
	  for(unsigned int i=0, j=m_vaoList.size(); i<j; i++ )
	  {
		m_materialList[i]->bind(shaderInfo, shaderProgram);
		m_vaoList[i]->render(GL_TRIANGLES);
		m_materialList[i]->unbind();
	  }
	}

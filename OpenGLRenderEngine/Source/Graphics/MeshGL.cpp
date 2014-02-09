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
			case BONE_BUFFER:
			{

glGetError();
				const BoneVertex* buffer = (const BoneVertex*) bufferArray; 
				m_byteSize = bufferArraySize*sizeof(BoneVertex);
				glBufferData(GL_ARRAY_BUFFER, m_byteSize, &buffer[0].m_boneID[0], GL_STATIC_DRAW);

if(glGetError()!=GL_NO_ERROR) throw exception("error");

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
		if(ptrVBOList.size() < 3)
			throw exception("ptrVBOList contains less than 3 elements.");
		
		glGenVertexArrays(1, &m_vaoID);
		glBindVertexArray(m_vaoID);

		{
			int vertexAttribLoc = 0;
			int normalAttribLoc = 1;
			int textCoordAttribLoc = 2;

			//vertex
			m_ptrVBO[0] = ptrVBOList[0];
			glBindBuffer(GL_ARRAY_BUFFER, ptrVBOList[0]->getBufferID() );
			glVertexAttribPointer( vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
			glEnableVertexAttribArray(vertexAttribLoc);

			//normal
			m_ptrVBO[1] = ptrVBOList[1];
			glBindBuffer(GL_ARRAY_BUFFER, ptrVBOList[1]->getBufferID() );
			glVertexAttribPointer( normalAttribLoc, 3, GL_FLOAT, GL_TRUE, 0, (void*)0 );
			glEnableVertexAttribArray(normalAttribLoc);

			//textcoord
			m_ptrVBO[2] = ptrVBOList[2];
			glBindBuffer(GL_ARRAY_BUFFER, ptrVBOList[2]->getBufferID() );
			glVertexAttribPointer( textCoordAttribLoc, 2, GL_FLOAT, GL_TRUE, 0, (void*)0 );
			glEnableVertexAttribArray(textCoordAttribLoc);
			
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ptrIBO->getIndexID());
		glVertexAttribPointer( 5, 3, GL_UNSIGNED_SHORT, GL_FALSE, 0, (void*)0 );
		glBindVertexArray(0);

	}

	VertexArrayObject::~VertexArrayObject()
	{
		glDeleteVertexArrays(1, &m_vaoID);
	}


	void VertexArrayObject::addVBO(const shared_ptr<VertexBufferObject>& ptrVBO)
	{
		glBindVertexArray(m_vaoID);

		if(ptrVBO->getType() == VertexBufferObject::BONE_BUFFER)
		{
			int boneIDAttribLoc = 3;
			int boneWeightAttribLoc = 4;

			m_ptrVBO[3] = ptrVBO;
			glBindBuffer(GL_ARRAY_BUFFER, m_ptrVBO[3]->getBufferID() );
			glVertexAttribIPointer( boneIDAttribLoc, 4, GL_INT, sizeof(BoneVertex), (void*)0 );
			glVertexAttribPointer( boneWeightAttribLoc, 4, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)(4*sizeof(int)) );

			glEnableVertexAttribArray(boneIDAttribLoc);
			glEnableVertexAttribArray(boneWeightAttribLoc);

		}

		glBindVertexArray(0);
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

  //===================================
  // BoneVertex
  //===================================

  BoneVertex::BoneVertex()
  {
	for(unsigned int i=0; i<MAX_BONES_PER_VERTEX; i++)
	{
		m_boneID[i] = 0;
		m_boneWeight[i] = 0.0f;
	}
  }

  void BoneVertex::addBone(unsigned int boneID, float boneWeight)
  {
	for(unsigned int i=0; i<MAX_BONES_PER_VERTEX; i++)
	{
		if(m_boneWeight[i] == 0.0f)
		{
			m_boneID[i] = boneID;
			m_boneWeight[i] = boneWeight;
			return;
		}
	}

	throw exception("exceeded bone limit.");
  }


  //===================================
  // AnimatedMeshGL
  //===================================

  AnimatedMeshGL::AnimatedMeshGL(const MeshData& meshData, const vector<MaterialData>& materialDataList)
	  : m_boneNameToIndex(meshData.m_boneNameToIndex), 
	    m_boneFinalTransform(meshData.m_boneNameToIndex.size()), m_bone(meshData.m_bone),
		m_ptrMesh( graphics.createMesh(meshData, materialDataList) )
  {

	  //Copy VertexBone data
	  unsigned int maxVertexBones = meshData.m_vertexBone.size();
	  vector<BoneVertex> boneVertexList(maxVertexBones);
	  for(unsigned int i=0; i<maxVertexBones; i++)
	  {
		  for(unsigned int j=0; j<meshData.m_vertexBone[i].m_weight.size(); j++)
		  {
			  boneVertexList[i].addBone(meshData.m_vertexBone[i].m_weight[j].first, 
				  meshData.m_vertexBone[i].m_weight[j].second);
		  }
	  }

	  shared_ptr<VertexBufferObject> ptrBoneVBO( new VertexBufferObject(VertexBufferObject::BONE_BUFFER, boneVertexList.size(), (const DataType)&boneVertexList[0]));
	  for(unsigned int i=0, size = m_ptrMesh->m_vaoList.size(); i<size; i++)
		  m_ptrMesh->m_vaoList[i]->addVBO( ptrBoneVBO );
	  

/*
		unsigned int maxVertices = meshData.m_vertex.size();
		unsigned int maxUsedMaterials = meshData.m_matRange.size(); //# of materials actually used by the mesh
		GraphicsGL& gfx = graphics;

		//load vertex data
		vector<shared_ptr<VertexBufferObject>> ptrVBOList;
		ptrVBOList.push_back(shared_ptr<VertexBufferObject>(new VertexBufferObject(VertexBufferObject::VERTEX_BUFFER, meshData.m_vertex.size(), (const DataType)&meshData.m_vertex[0])));
		ptrVBOList.push_back(shared_ptr<VertexBufferObject>(new VertexBufferObject(VertexBufferObject::NORMAL_BUFFER, meshData.m_normal.size(), (const DataType)&meshData.m_normal[0])));
		ptrVBOList.push_back(shared_ptr<VertexBufferObject>(new VertexBufferObject(VertexBufferObject::TEXTCOORD_BUFFER, meshData.m_textCoord.size(), (const DataType)&meshData.m_textCoord[0])));
		ptrVBOList.push_back(shared_ptr<VertexBufferObject>(new VertexBufferObject(VertexBufferObject::BONE_BUFFER, boneVertexList.size(), (const DataType)&boneVertexList[0])));

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

			m_materialList.push_back( materialPtr );
			m_matIndexList.push_back( i );
		}
//*/
  }

  AnimatedMeshGL::~AnimatedMeshGL()
  {
	  graphics.releaseMesh(m_ptrMesh);
  }


  	void AnimatedMeshGL::render(const SceneShaderInfo& shaderInfo, shared_ptr<ShaderProgramGL>& shaderProgram) const
	{
		m_ptrMesh->render(shaderInfo, shaderProgram);
	}

	void AnimatedMeshGL::getBoneTransformation(double timeEllasped, const Skeleton& skeleton, AnimationComponent& animComponent, vector<glm::mat4>& finalTransformList)
	{
		auto& anim = skeleton.getAnimation(animComponent.m_animationIndex);
		unsigned int keyFrameCount = anim.getKeyFrameCount();
		double ticksPerSecond = anim.getTicksPerSecond() != 0 ? anim.getTicksPerSecond() : 25.0f;
		double timeInTicks = timeEllasped * ticksPerSecond;
		double animationTime = fmod(timeInTicks, anim.getDuration());

		 for(unsigned int i = animComponent.m_startFrameIndex; i < keyFrameCount - 1 ; i++) 
		 {
			 if (animationTime < anim.getKeyFrame(i + 1).getTime()) {
				 animComponent.m_startFrameIndex = i;
				 break;
			}
		}
		animComponent.m_endFrameIndex = (animComponent.m_startFrameIndex + 1) % keyFrameCount;

		double deltaTime = (anim.getKeyFrame(animComponent.m_endFrameIndex).getTime() - anim.getKeyFrame(animComponent.m_startFrameIndex).getTime());
		animComponent.m_remainingTime = (animationTime - anim.getKeyFrame(animComponent.m_startFrameIndex).getTime()) / deltaTime;

		glm::mat4 identity(1.0f);
		transformBone(0, identity, skeleton, animComponent);

		for(unsigned int i=0, maxBones=m_boneFinalTransform.size(); i<maxBones; i++)
			finalTransformList[i] = m_boneFinalTransform[i];

		if(animComponent.m_startFrameIndex == keyFrameCount - 2)
			animComponent.m_startFrameIndex = 0;
	}

	void AnimatedMeshGL::transformBone(unsigned int boneID, const glm::mat4& parentTransform, const Skeleton& skeleton, const AnimationComponent& animComponent)
	{
		const Bone& node = skeleton.getBoneNode(boneID);		
		const Animation& animation = skeleton.getAnimation(animComponent.m_animationIndex);
		const KeyFrame& startKeyframe =	animation.getKeyFrame(animComponent.m_startFrameIndex);
		const KeyFrame& endKeyframe = animation.getKeyFrame(animComponent.m_endFrameIndex);

		const glm::mat4& inverseTransform = skeleton.getInverseTransform();
		glm::mat4 nodeTransform = node.m_transform;

		unsigned int boneIndex = animation.getBoneIndex(node.m_name);
		if(boneIndex != ~0)
		{
			float factor  = float( animComponent.m_remainingTime );

			auto rotQuat = glm::slerp(startKeyframe.getBoneRotation(boneIndex),
				endKeyframe.getBoneRotation(boneIndex), factor);

			auto posVec = BlendVec3(startKeyframe.getBoneTranslation(boneIndex),
				endKeyframe.getBoneTranslation(boneIndex), factor);

			glm::mat4 translation = glm::translate(glm::mat4(), posVec);
			nodeTransform = translation * glm::toMat4(rotQuat);
		}

		glm::mat4 localFinalTransform = parentTransform * nodeTransform;

		auto iter2 = m_boneNameToIndex.find(node.m_name);
		if(iter2!=m_boneNameToIndex.end())
		{
			glm::mat4 boneOffset = glm::make_mat4( m_bone[iter2->second].m_offsetMatrix.m_m16 );
			m_boneFinalTransform[iter2->second] = inverseTransform * localFinalTransform * boneOffset;
		}

		for(unsigned int i=0; i<node.m_childID.size(); i++)
			transformBone(node.m_childID[i], localFinalTransform, skeleton, animComponent);
	}




#include "FileReaderAssimp.h"
#include <cassert>
#include "../Globals.h"

bool AssimpMeshFileReader::load(const string& filename, MeshFileData& mfd)
{
	Assimp::Importer importer;
	//const aiScene* ptrScene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	const aiScene* ptrScene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if(!ptrScene)
		return false;

	map<string, MergeMeshData> meshMap;
	unsigned int meshIndex = 0;
	for(unsigned int i=0; i<ptrScene->mNumMeshes; i++)
	{
		string strName = string(ptrScene->mMeshes[i]->mName.data);
		map<string, MergeMeshData>::iterator iter = meshMap.find( strName );
		if(iter == meshMap.end())
		{
			MergeMeshData mmd(meshIndex, ptrScene->mMeshes[i]->mNumVertices, ptrScene->mMeshes[i]->mNumFaces, ptrScene->mMeshes[i]->mNumBones);
			meshMap.insert( pair<string, MergeMeshData>(strName, mmd) );
			meshIndex++;
		}
		else
		{
			iter->second.m_maxVertices += ptrScene->mMeshes[i]->mNumVertices;
			iter->second.m_maxTriFaces += ptrScene->mMeshes[i]->mNumFaces;
			iter->second.m_maxBones += ptrScene->mMeshes[i]->mNumBones;
		}
	}


	mfd.m_meshData.resize(meshMap.size());
	for(unsigned int i=0; i<ptrScene->mNumMeshes; i++)
	{
		loadMeshData(mfd.m_meshData, ptrScene->mMeshes[i], meshMap);
	}

	//Load Material
	mfd.m_materialData.resize(ptrScene->mNumMaterials);
	for (unsigned int i = 0 ; i < ptrScene->mNumMaterials ; i++) 
	{
		loadMaterial(mfd.m_materialData[i], ptrScene->mMaterials[i]);
	}


	if(ptrScene->mRootNode)
	{
		loadSkeletonNode(ptrScene->mRootNode, mfd.m_skeleton.m_nodes, mfd.m_skeleton.m_nodeNameToIndex);

		auto invMat = ptrScene->mRootNode->mTransformation;
		invMat.Inverse();
		invMat.Transpose();
		for(unsigned int i=0; i<4; i++)
		{
			for(unsigned int j=0; j<4; j++)
				mfd.m_skeleton.m_inverseTransform.m_m4x4[i][j] = invMat[i][j];
		}
	}

	if(ptrScene->HasAnimations())
	{
		mfd.m_skeleton.m_animation.resize(ptrScene->mNumAnimations);
		for(unsigned int i=0; i<ptrScene->mNumAnimations; i++)
		{
			loadAnimation(ptrScene->mAnimations[i], mfd.m_skeleton.m_animation[i]);

			if(mfd.m_skeleton.m_animation[i].m_name.empty())
			{
				mfd.m_skeleton.m_animation[i].m_name = "anim";
			}
		}
	}

	return true;
}


unsigned int AssimpMeshFileReader::loadSkeletonNode(aiNode* ptrAiNode, vector<SkeletonNodeData>& nodeList, map<string,unsigned int>& nodeNameMap)
{
	string aiNodeName = string(ptrAiNode->mName.data);
	//make sure the node is not already in the nodeList
	if(nodeNameMap.find(aiNodeName)!=nodeNameMap.end()) return ~0;

	unsigned int nodeIndex = nodeList.size();
	nodeList.push_back(SkeletonNodeData());	

	nodeList[nodeIndex].m_id = nodeIndex;
	nodeList[nodeIndex].m_name = aiNodeName;
	nodeList[nodeIndex].m_parentID = (ptrAiNode->mParent ? nodeNameMap[ptrAiNode->mParent->mName.data] : ~0);


	auto tempMat = ptrAiNode->mTransformation;
	tempMat.Transpose();
	for(unsigned int i=0; i<4; i++)
	{
		for(unsigned int j=0; j<4; j++)
			nodeList[nodeIndex].m_transform.m_m4x4[i][j] = tempMat[i][j];
	}

	unsigned int childIndex;
	nodeNameMap.insert(pair<string, unsigned int>(nodeList[nodeIndex].m_name,nodeList[nodeIndex].m_id));
	nodeList[nodeIndex].m_childNameList.resize(ptrAiNode->mNumChildren);
	for(unsigned int i=0; i<ptrAiNode->mNumChildren; i++)
	{
		childIndex = loadSkeletonNode(ptrAiNode->mChildren[i], nodeList, nodeNameMap);
		nodeList[nodeIndex].m_childNameList[i] = childIndex;
	}

	return nodeIndex;
}


void AssimpMeshFileReader::loadMeshData(vector<MeshData>& meshList, const aiMesh* ptrAiMesh, map<string, MergeMeshData>& meshMap)
{
	string strName = string(ptrAiMesh->mName.C_Str());
	map<string, MergeMeshData>::iterator iter = meshMap.find(strName);
	MeshData& mesh = meshList[iter->second.m_index];
	unsigned int vertexSizeOffset = iter->second.m_numVertices;
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	if(mesh.m_vertex.empty())
	{
		mesh.m_vertex.resize(iter->second.m_maxVertices);
		mesh.m_normal.resize(iter->second.m_maxVertices);
		mesh.m_textCoord.resize(iter->second.m_maxVertices);
		mesh.m_triIndex.resize(iter->second.m_maxTriFaces);

		mesh.m_vertexBone.resize(iter->second.m_maxVertices);
		mesh.m_bone.reserve(iter->second.m_maxBones);

		mesh.m_matIndexID.reserve(meshMap.size());
		mesh.m_matRange.reserve(meshMap.size()); 
	}

	if(ptrAiMesh->HasBones())
	{
		for(unsigned int i=0; i<ptrAiMesh->mNumBones; i++)
		{
			auto iter2 = mesh.m_boneNameToIndex.find(ptrAiMesh->mBones[i]->mName.data);
			unsigned int boneIndex;
			if(iter2 == mesh.m_boneNameToIndex.end())
			{
				boneIndex = mesh.m_bone.size();
				mesh.m_boneNameToIndex.insert(pair<string, unsigned int>(ptrAiMesh->mBones[i]->mName.data, boneIndex));

				mesh.m_bone.push_back(BoneData());
				mesh.m_bone[boneIndex].m_name = string(ptrAiMesh->mBones[i]->mName.data);

				auto tempMat = ptrAiMesh->mBones[i]->mOffsetMatrix;
				tempMat.Transpose();
				for(unsigned int j=0; j<4; j++)
				{
					for(unsigned int k=0; k<4; k++)
						mesh.m_bone[boneIndex].m_offsetMatrix.m_m4x4[j][k] = tempMat[j][k];
				}
			}
			else
			{
				boneIndex = iter2->second;
			}

			unsigned int vertexIndex;
			float weightBias;
			for(unsigned int j=0; j<ptrAiMesh->mBones[i]->mNumWeights; j++)
			{
				vertexIndex = ptrAiMesh->mBones[i]->mWeights[j].mVertexId + vertexSizeOffset;
				weightBias = ptrAiMesh->mBones[i]->mWeights[j].mWeight;
				mesh.m_vertexBone[vertexIndex].m_weight.push_back(pair<unsigned int,float>(boneIndex, weightBias));
			}
		}
	}

	for(unsigned int i=0; i<ptrAiMesh->mNumVertices; i++)
	{
		unsigned int pos = vertexSizeOffset + i;

		mesh.m_vertex[pos].m_x = ptrAiMesh->mVertices[i][0];
		mesh.m_vertex[pos].m_y = ptrAiMesh->mVertices[i][1];
		mesh.m_vertex[pos].m_z = ptrAiMesh->mVertices[i][2];

		mesh.m_normal[pos].m_x = ptrAiMesh->mNormals[i][0];
		mesh.m_normal[pos].m_y = ptrAiMesh->mNormals[i][1];
		mesh.m_normal[pos].m_z = ptrAiMesh->mNormals[i][2];

		const aiVector3D* pTexCoord = ptrAiMesh->HasTextureCoords(0) ? &(ptrAiMesh->mTextureCoords[0][i]) : &Zero3D;
		mesh.m_textCoord[pos].m_u = pTexCoord->x;
		mesh.m_textCoord[pos].m_v = pTexCoord->y;
	}

	for (unsigned int i = 0 ; i < ptrAiMesh->mNumFaces ; i++) 
	{
		unsigned int pos = iter->second.m_numTriFaces + i;
		const aiFace& Face = ptrAiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);

		mesh.m_triIndex[pos].m_a = Face.mIndices[0] + iter->second.m_numVertices;
		mesh.m_triIndex[pos].m_b = Face.mIndices[1] + iter->second.m_numVertices;
		mesh.m_triIndex[pos].m_c = Face.mIndices[2] + iter->second.m_numVertices;
	}

	iter->second.m_numVertices += ptrAiMesh->mNumVertices;
	iter->second.m_numTriFaces += ptrAiMesh->mNumFaces;

	mesh.m_matIndexID.push_back(ptrAiMesh->mMaterialIndex);
	mesh.m_matRange.push_back( ptrAiMesh->mNumFaces ); 
}

void AssimpMeshFileReader::loadMaterial(MaterialData& material, aiMaterial* pMaterial)
{
	//AI_MATKEY_NAME
	//AI_MATKEY_TWOSIDED 
	//AI_MATKEY_SHADING_MODEL 
	//AI_MATKEY_ENABLE_WIREFRAME 
	//AI_MATKEY_BLEND_FUNC 
	//AI_MATKEY_OPACITY 
	//AI_MATKEY_BUMPSCALING 
	//AI_MATKEY_SHININESS 
	//AI_MATKEY_REFLECTIVITY 
	//AI_MATKEY_SHININESS_STRENGTH 
	//AI_MATKEY_REFRACTI 
	//AI_MATKEY_COLOR_DIFFUSE 
	//AI_MATKEY_COLOR_AMBIENT 
	//AI_MATKEY_COLOR_SPECULAR 
	//AI_MATKEY_COLOR_EMISSIVE 
	//AI_MATKEY_COLOR_TRANSPARENT 
	//AI_MATKEY_COLOR_REFLECTIVE 
	//AI_MATKEY_GLOBAL_BACKGROUND_IMAGE

	aiString str;
	if(pMaterial->Get(AI_MATKEY_NAME, str) == AI_SUCCESS )
	{
		material.m_name = string(str.C_Str());
	}

	aiColor4D color;
	if(AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_AMBIENT, &color))
	{
		material.m_ambient.m_r = color.r;
		material.m_ambient.m_g = color.g;
		material.m_ambient.m_b = color.b;
		material.m_ambient.m_a = color.a;
	}

	if(AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &color))
	{
		material.m_diffuse.m_r = color.r;
		material.m_diffuse.m_g = color.g;
		material.m_diffuse.m_b = color.b;
		material.m_diffuse.m_a = color.a;
	}


	if(AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_SPECULAR, &color))
	{
		material.m_specular.m_r = color.r;
		material.m_specular.m_g = color.g;
		material.m_specular.m_b = color.b;
		material.m_specular.m_a = color.a;
	}

	if(AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_EMISSIVE, &color))
	{
		material.m_emission.m_r = color.r;
		material.m_emission.m_g = color.g;
		material.m_emission.m_b = color.b;
		material.m_emission.m_a = color.a;
	}

	unsigned int maxDiffuseTextures = pMaterial->GetTextureCount(aiTextureType_DIFFUSE);
	for(unsigned int i=0; i<maxDiffuseTextures; i++)
	{
		if (pMaterial->GetTexture(aiTextureType_DIFFUSE, i, &str, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
		{
			TextureData textdata;
			textdata.m_id = TextureData::DIFFUSE_TEXTURE;
			textdata.m_filename = str.C_Str();

			material.m_textureList.push_back(textdata);
		}
	}
}

void AssimpMeshFileReader::loadAnimation(aiAnimation* ptrAiAnimation, AnimationData& animation)
{
	unsigned int i,j;
	animation.m_name = string(ptrAiAnimation->mName.data);
	animation.m_duration = ptrAiAnimation->mDuration;
	animation.m_ticksPerSecond = ptrAiAnimation->mTicksPerSecond;

	unsigned int npos = ptrAiAnimation->mChannels[0]->mNumPositionKeys;
	unsigned int nrot = ptrAiAnimation->mChannels[0]->mNumRotationKeys;

	animation.m_node.resize(ptrAiAnimation->mNumChannels);
	for(i=0; i<ptrAiAnimation->mNumChannels; i++)
	{
		animation.m_node[i].m_equalFrames = (ptrAiAnimation->mChannels[i]->mNumPositionKeys == 
											 ptrAiAnimation->mChannels[i]->mNumRotationKeys);

		if(npos != ptrAiAnimation->mChannels[i]->mNumPositionKeys) animation.m_node[i].m_equalFrames = false;
		if(nrot != ptrAiAnimation->mChannels[i]->mNumRotationKeys) animation.m_node[i].m_equalFrames = false;

		animation.m_node[i].m_name = string(ptrAiAnimation->mChannels[i]->mNodeName.data);
		animation.m_nodeNameToIndexMap.insert(pair<string,unsigned int>(animation.m_node[i].m_name,i));

		animation.m_node[i].m_positionFrame.resize(ptrAiAnimation->mChannels[i]->mNumPositionKeys);
		for(j=0; j<ptrAiAnimation->mChannels[i]->mNumPositionKeys; j++)
		{
			//fileLogger.print("posFrame[%d] time: %g\n", j, ptrAiAnimation->mChannels[i]->mPositionKeys[j].mTime);

			animation.m_node[i].m_positionFrame[j].first = ptrAiAnimation->mChannels[i]->mPositionKeys[j].mTime;
			animation.m_node[i].m_positionFrame[j].second.m_x = ptrAiAnimation->mChannels[i]->mPositionKeys[j].mValue.x;
			animation.m_node[i].m_positionFrame[j].second.m_y = ptrAiAnimation->mChannels[i]->mPositionKeys[j].mValue.y;
			animation.m_node[i].m_positionFrame[j].second.m_z = ptrAiAnimation->mChannels[i]->mPositionKeys[j].mValue.z;
		}

		animation.m_node[i].m_rotationFrame.resize(ptrAiAnimation->mChannels[i]->mNumRotationKeys);
		for(j=0; j<ptrAiAnimation->mChannels[i]->mNumRotationKeys; j++)
		{
			//fileLogger.print("rotFrame[%d] time: %g\n", j, ptrAiAnimation->mChannels[i]->mRotationKeys[j].mTime);

			animation.m_node[i].m_rotationFrame[j].first = ptrAiAnimation->mChannels[i]->mRotationKeys[j].mTime;
			animation.m_node[i].m_rotationFrame[j].second.m_w = ptrAiAnimation->mChannels[i]->mRotationKeys[j].mValue.w;
			animation.m_node[i].m_rotationFrame[j].second.m_x = ptrAiAnimation->mChannels[i]->mRotationKeys[j].mValue.x;
			animation.m_node[i].m_rotationFrame[j].second.m_y = ptrAiAnimation->mChannels[i]->mRotationKeys[j].mValue.y;
			animation.m_node[i].m_rotationFrame[j].second.m_z = ptrAiAnimation->mChannels[i]->mRotationKeys[j].mValue.z;

		}

	}

}

#pragma once
#include "../TypeDef.h"
#include "MeshData.h"
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>



	class AssimpMeshFileReader
	{
		public:

		struct MergeMeshData
		{
			MergeMeshData(unsigned int index, unsigned int maxVertices, unsigned int maxTriFaces, unsigned int maxBones)
			: m_index(index), m_maxVertices(maxVertices), m_maxTriFaces(maxTriFaces), m_maxBones(maxBones),
			  m_numVertices(0), m_numTriFaces(0), m_numBones(0)
			{
			}

			unsigned int m_index;
			unsigned int m_maxVertices;
			unsigned int m_maxTriFaces;
			unsigned int m_maxBones;

			unsigned int m_numVertices;
			unsigned int m_numTriFaces;
			unsigned int m_numBones;
		};

		bool load(const string& filename, MeshFileData& mfd);

		void loadMeshData(vector<MeshData>& meshList, const aiMesh* ptrAiMesh, map<string, MergeMeshData>& meshMap);
		void loadMeshBoneData(BoneData& bone, const aiBone* ptrAiBone);
		void loadMaterial(MaterialData& material, aiMaterial* pMaterial);

		unsigned int loadSkeletonNode(aiNode* ptrAiNode, vector<SkeletonNodeData>& nodeList, map<string,unsigned int>& nodeNameMap);
		void loadAnimation(aiAnimation* ptrAiAnimation, AnimationData& animation);
	};

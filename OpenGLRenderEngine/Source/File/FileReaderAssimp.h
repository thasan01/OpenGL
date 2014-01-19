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
			MergeMeshData(unsigned int index, unsigned int m_maxVertices, unsigned int m_maxTriFaces)
			: m_index(index), m_maxVertices(m_maxVertices), m_maxTriFaces(m_maxTriFaces), m_numVertices(0), m_numTriFaces(0)
			{
			}

			unsigned int m_index;
			unsigned int m_maxVertices;
			unsigned int m_maxTriFaces;

			unsigned int m_numVertices;
			unsigned int m_numTriFaces;
		};

		bool load(const string& filename, MeshFileData& mfd, const float* transformMatrix);

		void loadMeshData(vector<MeshData>& meshList, const aiMesh* paiMesh, map<string, MergeMeshData>& meshMap);
		void loadMaterial(MaterialData& material, aiMaterial* pMaterial);
	};

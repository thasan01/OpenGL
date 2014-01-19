#include "FileReaderAssimp.h"
#include <cassert>

bool AssimpMeshFileReader::load(const string& filename, MeshFileData& mfd, const float* transformMatrix)
{
	Assimp::Importer importer;
	//const aiScene* pScene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	const aiScene* pScene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if(!pScene)
		return false;

	map<string, MergeMeshData> meshMap;
	unsigned int meshIndex = 0;
	for(unsigned int i=0; i<pScene->mNumMeshes; i++)
	{
		string strName = string(pScene->mMeshes[i]->mName.data);
		map<string, MergeMeshData>::iterator iter = meshMap.find( strName );
		if(iter == meshMap.end())
		{
			MergeMeshData mmd(meshIndex, pScene->mMeshes[i]->mNumVertices, pScene->mMeshes[i]->mNumFaces);
			meshMap.insert( pair<string, MergeMeshData>(strName, mmd) );
			meshIndex++;
		}
		else
		{
			iter->second.m_maxVertices += pScene->mMeshes[i]->mNumVertices;
			iter->second.m_maxTriFaces += pScene->mMeshes[i]->mNumFaces;
		}
	}


	mfd.m_meshData.resize(meshMap.size());
	for(unsigned int i=0; i<pScene->mNumMeshes; i++)
	{
		loadMeshData(mfd.m_meshData, pScene->mMeshes[i], meshMap);
	}

	//Load Material
	mfd.m_materialData.resize(pScene->mNumMaterials);
	for (unsigned int i = 0 ; i < pScene->mNumMaterials ; i++) 
	{
		loadMaterial(mfd.m_materialData[i], pScene->mMaterials[i]);
	}

	if(transformMatrix!= NULL)
	{
		float tempx, tempy, tempz;
		for(unsigned int i=0; i<mfd.m_meshData.size(); i++)
		{
			for(unsigned int j=0; j<mfd.m_meshData[i].m_vertex.size(); j++)
			{
				tempx = mfd.m_meshData[i].m_vertex[j].m_x;
				tempy = mfd.m_meshData[i].m_vertex[j].m_y;
				tempz = mfd.m_meshData[i].m_vertex[j].m_z;

				mfd.m_meshData[i].m_vertex[j].m_x = (tempx * transformMatrix[0]) + (tempy * transformMatrix[4]) + (tempz * transformMatrix[8]) + transformMatrix[12];
				mfd.m_meshData[i].m_vertex[j].m_y = (tempx * transformMatrix[1]) + (tempy * transformMatrix[5]) + (tempz * transformMatrix[9]) + transformMatrix[13];
				mfd.m_meshData[i].m_vertex[j].m_z = (tempx * transformMatrix[2]) + (tempy * transformMatrix[6]) + (tempz * transformMatrix[10]) + transformMatrix[14];
			}

			for(unsigned int j=0; j<mfd.m_meshData[i].m_normal.size(); j++)
			{
				tempx = mfd.m_meshData[i].m_normal[j].m_x;
				tempy = mfd.m_meshData[i].m_normal[j].m_y;
				tempz = mfd.m_meshData[i].m_normal[j].m_z;

				mfd.m_meshData[i].m_normal[j].m_x = (tempx * transformMatrix[0]) + (tempy * transformMatrix[4]) + (tempz * transformMatrix[8]);
				mfd.m_meshData[i].m_normal[j].m_y = (tempx * transformMatrix[1]) + (tempy * transformMatrix[5]) + (tempz * transformMatrix[9]);
				mfd.m_meshData[i].m_normal[j].m_z = (tempx * transformMatrix[2]) + (tempy * transformMatrix[6]) + (tempz * transformMatrix[10]);
			}
		}
	}

	return true;
}


void AssimpMeshFileReader::loadMeshData(vector<MeshData>& meshList, const aiMesh* paiMesh, map<string, MergeMeshData>& meshMap)
{
	string strName = string(paiMesh->mName.C_Str());
	map<string, MergeMeshData>::iterator iter = meshMap.find(strName);
	MeshData& mesh = meshList[iter->second.m_index];


	if(mesh.m_vertex.empty())
	{
		mesh.m_vertex.resize(iter->second.m_maxVertices);
		mesh.m_normal.resize(iter->second.m_maxVertices);
		mesh.m_textCoord.resize(iter->second.m_maxVertices);
		mesh.m_triIndex.resize(iter->second.m_maxTriFaces);

		mesh.m_matIndexID.reserve(meshMap.size());
		mesh.m_matRange.reserve(meshMap.size()); 
	}

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	for(unsigned int i=0; i<paiMesh->mNumVertices; i++)
	{
		unsigned int pos = iter->second.m_numVertices + i;

		mesh.m_vertex[pos].m_x = paiMesh->mVertices[i][0];
		mesh.m_vertex[pos].m_y = paiMesh->mVertices[i][1];
		mesh.m_vertex[pos].m_z = paiMesh->mVertices[i][2];

		mesh.m_normal[pos].m_x = paiMesh->mNormals[i][0];
		mesh.m_normal[pos].m_y = paiMesh->mNormals[i][1];
		mesh.m_normal[pos].m_z = paiMesh->mNormals[i][2];

		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
		mesh.m_textCoord[pos].m_u = pTexCoord->x;
		mesh.m_textCoord[pos].m_v = pTexCoord->y;
	}

	for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) 
	{
		unsigned int pos = iter->second.m_numTriFaces + i;
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);

		mesh.m_triIndex[pos].m_a = Face.mIndices[0] + iter->second.m_numVertices;
		mesh.m_triIndex[pos].m_b = Face.mIndices[1] + iter->second.m_numVertices;
		mesh.m_triIndex[pos].m_c = Face.mIndices[2] + iter->second.m_numVertices;
	}

	iter->second.m_numVertices += paiMesh->mNumVertices;
	iter->second.m_numTriFaces += paiMesh->mNumFaces;

	mesh.m_matIndexID.push_back(paiMesh->mMaterialIndex);
	mesh.m_matRange.push_back( paiMesh->mNumFaces ); 
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

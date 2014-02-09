#pragma once
#include "../TypeDef.h"


//use unsigned short for triangle indices, ould be changed to unsigned int if needed.
typedef	unsigned short TriIndexType;

struct Matrix4 {
	union { 
		float m_m4x4[4][4];
		float m_m16[16];
		struct { float  m_m11, m_m12, m_m13, m_14,
						m_m21, m_m22, m_m23, m_24,
						m_m31, m_m32, m_m33, m_34,
						m_m41, m_m42, m_m43, m_44; };
	};
};

struct Vertex3 {
	union {  
		struct{float m_x,m_y,m_z;};
		float m_v[3];
	};
};

struct Vertex4 {
	union {  
		struct{float m_w,m_x,m_y,m_z;};
		float m_v[4];
	};
};


struct TextCoord {
	union {  
		struct{float m_u,m_v;};
		float m_t[2];
	};
};

struct TriIndex {
	union {  
		struct{TriIndexType m_a,m_b,m_c;};
		TriIndexType m_i[3];
	};
};

struct Color3 {
	union {  
		struct{float m_r,m_g,m_b;};
		float m_c[3];
	};
};

struct Color4 {
	union {  
		struct{float m_r,m_g,m_b,m_a;};
		float m_c[4];
	};
};

struct AnimationNodeData
{
	string m_name;
	bool m_equalFrames; //true if m_positionFrame.size() == m_rotationFrame.size()
	vector<pair<double,Vertex3>> m_positionFrame;
	vector<pair<double,Vertex4>> m_rotationFrame;
};

struct AnimationData
{
	string m_name;
	double m_duration;
	double m_ticksPerSecond;
	map<string, unsigned int> m_nodeNameToIndexMap;
	vector<AnimationNodeData> m_node;
};

struct SkeletonNodeData
{
	unsigned int m_id;
	unsigned int m_parentID;
	string m_name;	
	vector<unsigned int> m_childNameList;
	Matrix4 m_transform; //transforms from node space to parent space
};

struct SkeletonData
{
	string m_name;
	map<string, unsigned int> m_nodeNameToIndex;
	vector<SkeletonNodeData> m_nodes;

	Matrix4 m_inverseTransform;
	vector<AnimationData>	m_animation;
};

//********************************************************************************************************
// BoneData
// Contains bone information for a mesh
//********************************************************************************************************
struct BoneData
{
	string m_name;
	Matrix4 m_offsetMatrix;
};

//********************************************************************************************************
// VertexBoneData
// For each Vertex of a Mesh, this struct contains the index of BoneData, and the weight bais.
//********************************************************************************************************
struct VertexBoneData
{
	//boneID + weightBias
	vector<pair<unsigned int, float>> m_weight;
};

struct MeshData
{
	string				m_name;

	//stores the # of TriFaces for each Material
	vector<unsigned int> m_matRange; 
	//stores the Material index of the materials in m_matRange
	vector<unsigned int> m_matIndexID; 

	//static mesh data
	vector<Vertex3>		m_vertex;
	vector<Vertex3>		m_normal;
	vector<TextCoord>	m_textCoord;
	vector<TriIndex>	m_triIndex;
	
	//animated mesh data
	vector<BoneData>		m_bone;
	vector<VertexBoneData>	m_vertexBone;
	map<string, unsigned int> m_boneNameToIndex;
};

struct TextureData
{
	enum Type{	AMBIENT_TEXTURE, DIFFUSE_TEXTURE, 
		SPECULAR_TEXTURE, BUMP_MAP_TEXTURE };

	Type			m_type;
	unsigned int	m_id;	//used for multi-textures
	string			m_filename;
};

struct MaterialData
{
	string	m_name;
	string	m_shaderName;
	Color4	m_ambient;
	Color4	m_diffuse;
	Color4	m_specular;
	Color4	m_emission;
	float	m_shininess;

	vector<TextureData>	m_textureList;
};

struct MeshFileData
{
	vector<MaterialData>	m_materialData;
	vector<MeshData>		m_meshData;
	SkeletonData			m_skeleton;
};


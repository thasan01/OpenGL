#pragma once
#include "../TypeDef.h"


//use unsigned short for triangle indices, ould be changed to unsigned int if needed.
typedef	unsigned short TriIndexType;

struct Vertex3 {
	union {  
		struct{float m_x,m_y,m_z;};
		float m_v[3];
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

struct MeshData
{
	string				m_name;

	//stores the # of TriFaces for each Material
	vector<unsigned int> m_matRange; 
	//stores the Material index of the materials in m_matRange
	vector<unsigned int> m_matIndexID; 

	vector<Vertex3>		m_vertex;
	vector<Vertex3>		m_normal;
	vector<TextCoord>	m_textCoord;
	vector<TriIndex>	m_triIndex;
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
};


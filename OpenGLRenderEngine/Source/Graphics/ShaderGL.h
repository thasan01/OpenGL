#pragma once
#include "../TypeDef.h"
#include <GL\glew.h>
#include <GL\glu.h>
#include <GL\gl.h>

class ShaderGL;
class ShaderVariableGL;
class SaderBlockGL;
class ShaderProgramGL;

	class SceneShaderInfo
	{
		public:
		SceneShaderInfo();
		SceneShaderInfo(int modelMatrixLocation, int projModelViewMatrixLocation, int normalMatrixLocation, int materialAmbientLocation, int materialDiffuseLocation, int materialSpecularLocation, int materialShininessLocation, int lightAmbientLocation, int lightDiffuseLocation, int lightSpecularLocation, int lightPositionLocation);

		int m_modelMatrixLocation;
		int m_projModelViewMatrixLocation;
		int m_normalMatrixLocation;


		int	m_materialAmbientLocation;
		int	m_materialDiffuseLocation;
		int	m_materialSpecularLocation;
		int	m_materialShininessLocation;

		int	m_lightAmbientLocation;
		int	m_lightDiffuseLocation;
		int	m_lightSpecularLocation;
		int	m_lightPositionLocation;
	};


class ShaderGL
{
public:
	static bool loadShaderFromFile(const string& file, string& source);

	enum Type {VERTEX_SHADER, FRAGMENT_SHADER};
	unsigned int m_shaderID;
	string m_filename;
};

class ShaderVariableGL
{
public:
	ShaderVariableGL(const string& name, unsigned int byteSize, const void* source);

	string			m_name;
	unsigned int	m_byteSize;
	const void*		m_source;
};

class ShaderVariableTypeGL
{
	public:
	ShaderVariableTypeGL() {}

	ShaderVariableTypeGL( string name, string type, unsigned int location, int size)
		: m_name(name), m_type(type), m_location(location), m_size(size)	
	{
	}


	string m_name;
	string m_type;

	unsigned int m_location;
	int m_size;	
};

class ShaderBlockGL
{
public:
	ShaderBlockGL(ShaderProgramGL& shaderProgram, const string& blockName, const vector<ShaderVariableGL>& variableList);
	virtual ~ShaderBlockGL();

	bool setVariableValue(const string& variableName, const void* variableSource);
	bool setVariableValue(unsigned int locationID, unsigned int byteSize, const void* variableSource);
	bool getVariableValue(const string& variableName, void* variableSource);
	bool getVariableValue(unsigned int locationID, unsigned int byteSize, void* variableSource);
	bool getVariableLocation(const string& variableName, unsigned int& locationID) const;

	inline const string& getName() const { return m_blockName; }
	inline unsigned int getBlockID() const { return m_blockID; } 
	inline unsigned int getBufferID() const { return m_bufferObjectID; }

protected:
	string m_blockName;
	unsigned int m_blockID;
	unsigned int m_bufferObjectID;


	map<string, unsigned int>	m_nameToIndex;
	vector<unsigned int>		m_variableIndex;
	vector<unsigned int>		m_variableByteSize;
	vector<int>					m_variableOffset;
};


class ShaderProgramGL
{
	public:
	#define	INVALID_FILE_ERROR_MSG		"Program[%s]: Unable to load shader from file[%s]."
	#define	MISSING_SHADER_ERROR_MSG	"Program[%s]: No %s shader file provided."
	#define	SHADER_COMPILE_ERROR_MSG	"Shader[%s]:\n%s"
	#define	PROGRAM_COMPILE_ERROR_MSG	"Program[%s]:\n%s"

	ShaderProgramGL(const string& shaderProgramName, 
		const vector<string>& predefinedVariableNameList, 
		const vector<shared_ptr<ShaderGL>>& vertexShaderList, 
		const vector<shared_ptr<ShaderGL>>& fragmentShaderList);

	~ShaderProgramGL();

	const string& getName() const { return m_name; }
	inline bool isSuccess() const { return m_errorLog.empty(); }
	const char* getErrorMsg(unsigned int n)const;

	unsigned int getProgramID() const { return m_programID; }

	vector<ShaderVariableTypeGL> getAttributeVariableInfo() const;
	vector<ShaderVariableTypeGL> getUniformVariableInfo() const;

	void bind();
	void unbind();

	int getVariableLocation(const string& variableName) const;

	void setVariableInteger(int variableID, int value);
	void setVariableUnsignedInteger(int variableID, unsigned int value);
	void setVariableFloat(int variableID, float value);
	void setVariableVector3f(int variableID, const float value[3]);
	void setVariableVector4f(int variableID, const float value[4]);
	void setVariableMatrix3f(int variableID, const float value[9]);
	void setVariableMatrix4f(int variableID, const float value[16]);

	void addShaderBlock(const ShaderBlockGL& block);
	bool bindShaderBlock(const ShaderBlockGL& block) const;

	private:
	unsigned int m_programID;
	vector<string> m_errorLog;
	string m_name;

	std::vector<shared_ptr<ShaderGL>> m_vertexShader;
	std::vector<shared_ptr<ShaderGL>> m_fragmentShader;
	std::map<string, int> m_blockBindingIDMap;
};

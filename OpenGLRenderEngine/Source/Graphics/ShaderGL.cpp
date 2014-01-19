#include "ShaderGL.h"
#include "../Globals.h"

//======================================
// SceneShaderInfo
//======================================

	SceneShaderInfo::SceneShaderInfo()
	:	m_modelMatrixLocation(-1),
		m_projModelViewMatrixLocation(-1),
		m_normalMatrixLocation(-1),
		m_materialAmbientLocation(-1), 
		m_materialDiffuseLocation(-1), 
		m_materialSpecularLocation(-1), 
		m_materialShininessLocation(-1),
		m_lightAmbientLocation(-1),	
		m_lightDiffuseLocation(-1), 
		m_lightSpecularLocation(-1), 
		m_lightPositionLocation(-1)
	{
	}

	SceneShaderInfo::SceneShaderInfo(	int modelMatrixLocation, 
										int projModelViewMatrixLocation, 
										int normalMatrixLocation, 
										int materialAmbientLocation, 
										int materialDiffuseLocation, 
										int materialSpecularLocation, 
										int materialShininessLocation, 
										int lightAmbientLocation, 
										int lightDiffuseLocation, 
										int lightSpecularLocation, 
										int lightPositionLocation)
		:	m_modelMatrixLocation(modelMatrixLocation),
			m_projModelViewMatrixLocation(projModelViewMatrixLocation),
			m_normalMatrixLocation(normalMatrixLocation),
			m_materialAmbientLocation(materialAmbientLocation),
			m_materialDiffuseLocation(materialDiffuseLocation),
			m_materialSpecularLocation(materialSpecularLocation),
			m_materialShininessLocation(materialShininessLocation),
			m_lightAmbientLocation(lightAmbientLocation),
			m_lightDiffuseLocation(lightDiffuseLocation),
			m_lightSpecularLocation(lightSpecularLocation),
			m_lightPositionLocation(lightPositionLocation)
	{	
	}



//======================================
// ShaderGL
//======================================

  bool ShaderGL::loadShaderFromFile(const string& file, string& source)
  {
	std::ifstream t(file.data());

	if((void*)t == 0)
      return false;

	t.seekg(0, std::ios::end);   
	source.reserve(unsigned int(t.tellg()));
	t.seekg(0, int(std::ios::beg));

	source.assign((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());
	  return true;
  }

//======================================
// ShaderGL
//======================================


//======================================
// ShaderBlockGL
//======================================



//======================================
// ShaderProgramGL
//======================================

  ShaderProgramGL::ShaderProgramGL(const string& shaderProgramName, 
	  const vector<string>& predefinedVariableNameList, 
	  const vector<shared_ptr<ShaderGL>>& vertexShaderList, 
	  const vector<shared_ptr<ShaderGL>>& fragmentShaderList)
	:m_name(shaderProgramName), m_programID(glCreateProgram()),
	 m_vertexShader(vertexShaderList), m_fragmentShader(fragmentShaderList)
  {
	glGetError();//reset the error flag
	#define BUFFER_SIZE		262144
	char buffer[BUFFER_SIZE];
	char buffer2[BUFFER_SIZE];
	int buffer_length=0;

	  unsigned int maxShaders = m_vertexShader.size();
	  for(unsigned int i=0; i<maxShaders; i++)
	  {
		  glGetShaderInfoLog(m_vertexShader[i]->m_shaderID, BUFFER_SIZE, &buffer_length, buffer );

			if(buffer_length == 0)
				glAttachShader(m_programID, m_vertexShader[i]->m_shaderID);
			else
			{
				sprintf_s ( buffer2, SHADER_COMPILE_ERROR_MSG, m_vertexShader[i]->m_filename.data(), buffer );
				m_errorLog.push_back( buffer2 );
			}

	  }

	  maxShaders = m_fragmentShader.size();
	  for(unsigned int i=0; i<maxShaders; i++)
	  {
		  glGetShaderInfoLog(m_fragmentShader[i]->m_shaderID, BUFFER_SIZE, &buffer_length, buffer );

			if(buffer_length == 0)
				glAttachShader(m_programID, m_fragmentShader[i]->m_shaderID);
			else
			{
				sprintf_s ( buffer2, SHADER_COMPILE_ERROR_MSG, m_fragmentShader[i]->m_filename.data(), buffer );
				m_errorLog.push_back( buffer2 );
			}
	  }

	for(unsigned int i=0; i<predefinedVariableNameList.size(); i++)
		glBindAttribLocation(m_programID, i, predefinedVariableNameList[i].data());

	glLinkProgram(m_programID);

	GLint status;
	glGetProgramiv(m_programID,  GL_LINK_STATUS,  &status);
	if(status ==  GL_FALSE)
	{
		sprintf_s ( buffer2, "Failed to link program[%s]", m_name.data() );
		m_errorLog.push_back( buffer2 );
	}
	glUseProgram(0);

  }

ShaderProgramGL::~ShaderProgramGL()
{
	  unsigned int shaderID,i;
	  GraphicsGL& gfx = graphics;
	  for(i=0; i< m_vertexShader.size(); i++)
	  {
		  shaderID = m_vertexShader[i]->m_shaderID;
		  gfx.releaseShader(m_vertexShader[i]);
		  glDetachShader(m_programID, shaderID);
	  }
	  for(i=0; i< m_fragmentShader.size(); i++)
	  {
		  shaderID = m_fragmentShader[i]->m_shaderID;
		  gfx.releaseShader(m_fragmentShader[i]);
		  glDetachShader(m_programID, shaderID);
	  }
	  glDeleteProgram(m_programID);
}


  const char* ShaderProgramGL::getErrorMsg(unsigned int n)const
  {
	  return ( m_errorLog.size() > n ? m_errorLog[n].data() : NULL  );
  }


  void ShaderProgramGL::bind()
  {
	glUseProgram(m_programID);
  }

  void ShaderProgramGL::unbind()
  {
	glUseProgram(0);
  }


  int ShaderProgramGL::getVariableLocation(const string& variableName) const
  {
	 return glGetUniformLocation(m_programID, variableName.data());
  }


  void ShaderProgramGL::setVariableInteger(int variableID, int value)
  {
	glUniform1iv(variableID,  1, &value);
  }

  void ShaderProgramGL::setVariableUnsignedInteger(int variableID, unsigned int value)
  {
	glUniform1uiv(variableID,  1,  &value);
  }

  void ShaderProgramGL::setVariableFloat(int variableID, float value)
  {
	glUniform1fv(variableID,  1,  &value);
  }

  void ShaderProgramGL::setVariableVector3f(int variableID, const float value[3])
  {
	glUniform3fv(variableID,  1,  value);
  }

  void ShaderProgramGL::setVariableVector4f(int variableID, const float value[4])
  {
	glUniform4fv(variableID,  1,  value);
  }

  void ShaderProgramGL::setVariableMatrix3f(int variableID, const float value[9])
  {
	glUniformMatrix3fv(variableID,  1,  false,  value);
  }

  void ShaderProgramGL::setVariableMatrix4f(int variableID, const float value[16])
  {
	glUniformMatrix4fv(variableID,  1,  false,  value);
  }

  void ShaderProgramGL::addShaderBlock(const ShaderBlockGL& block)
  {
	  map<string, int>::const_iterator iter = m_blockBindingIDMap.find(block.getName());
	  if(iter == m_blockBindingIDMap.end())
	  {
		unsigned int id = m_blockBindingIDMap.size();
		m_blockBindingIDMap[block.getName()] = id;
		glUniformBlockBinding(m_programID, block.getBlockID(), id);
	  }
  }

  bool ShaderProgramGL::bindShaderBlock(const ShaderBlockGL& block) const
  {
	  map<string, int>::const_iterator iter = m_blockBindingIDMap.find(block.getName());
	  if(iter == m_blockBindingIDMap.end()) return false;

	  glBindBufferBase( GL_UNIFORM_BUFFER, iter->second, block.getBufferID() );
	  return true;
  }

  void ShaderProgramGL::getInputVariableInfo( vector<string>& variableNameList, 
											vector<string>& variableTypeList, 
											vector<unsigned int>& variableLocationList, 
											vector<int>& variableSize) const
  {
	  unsigned int programID = this->getProgramID();
	  GLint numAttributes, maxLength;
	  glGetProgramiv(programID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
	  glGetProgramiv(programID, GL_ACTIVE_ATTRIBUTES, &numAttributes);


	  char* name = new char[maxLength];
	  variableNameList.resize(numAttributes);
	  variableLocationList.resize(numAttributes);
	  variableSize.resize(numAttributes);

	  GLsizei nameSize;
	  GLenum type;
	  for(int i=0; i<numAttributes; i++)
	  {
		glGetActiveAttrib(programID,  i,  maxLength,  &nameSize,  &variableSize[i],  &type,  name);
		variableNameList[i] = string(name);
		variableLocationList[i] = glGetAttribLocation(programID, name);
	  }
	  delete name;
  }
  
  void ShaderProgramGL::getUniformVariableInfo( vector<string>& variableNameList, 
												vector<string>& variableTypeList,
												vector<unsigned int>& variableLocationList,
												vector<int>& variableSize) const
  {
	  unsigned int programID = this->getProgramID();
	  GLint numUniforms, maxLength;
	  glGetProgramiv(programID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
	  glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &numUniforms);


	  char* name = new char[maxLength];
	  variableNameList.resize(numUniforms);
	  variableLocationList.resize(numUniforms);
	  variableSize.resize(numUniforms);

	  GLsizei nameSize;
	  GLenum type;
	  for(int i=0; i<numUniforms; i++)
	  {
		glGetActiveUniform(programID,  i,  maxLength,  &nameSize,  &variableSize[i],  &type,  name);
		variableNameList[i] = string(name);
		variableLocationList[i] = glGetUniformLocation(programID, name);
	  }
	  delete name;
  }


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




//**************************************************
//	ShaderVariable
//**************************************************
  ShaderVariableGL::ShaderVariableGL(const string& name, unsigned int byteSize, const void* source)
	:m_name(name), m_byteSize(byteSize), m_source(source)
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
// ShaderBlockGL
//======================================

  ShaderBlockGL::ShaderBlockGL(ShaderProgramGL& shaderProgram, const string& blockName, const vector<ShaderVariableGL>& variableList)
	  :m_blockName(blockName), m_variableIndex(variableList.size()), m_variableOffset(variableList.size()), m_variableByteSize(variableList.size())
  {
	unsigned int programID = shaderProgram.getProgramID();
	GLint blockSize;
	m_blockID = glGetUniformBlockIndex(programID, blockName.data());

	if(m_blockID == GL_INVALID_INDEX)
		throw exception("error");

	glGetActiveUniformBlockiv(programID, m_blockID, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	if(blockSize == GL_INVALID_ENUM)
		throw exception("error");

	GLubyte* blockBuffer= (GLubyte *) malloc(blockSize);	

	unsigned int maxVariables = variableList.size(); 

	GLchar **var_names = new GLchar*[maxVariables];
	for(unsigned int i=0; i<maxVariables; i++)
	{
		var_names[i] = new GLchar[255];
		string newName = blockName + "." +variableList[i].m_name;
		memcpy ( var_names[i], newName.data(), strlen(newName.data())+1 );
	}
	glGetUniformIndices(programID, maxVariables, (const GLchar**)var_names, &m_variableIndex[0]);


	glGetActiveUniformsiv(programID, maxVariables, &m_variableIndex[0], GL_UNIFORM_OFFSET, &m_variableOffset[0]);
//*
	for(unsigned int i=0; i<maxVariables; i++)
	{
		m_nameToIndex[variableList[i].m_name] = i;
		m_variableByteSize[i] = variableList[i].m_byteSize;
		memcpy(blockBuffer + m_variableOffset[i], variableList[i].m_source, variableList[i].m_byteSize);
	}
//*/
	glGenBuffers( 1, &m_bufferObjectID );
	glBindBuffer( GL_UNIFORM_BUFFER, m_bufferObjectID );
	glBufferData( GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW );


	shaderProgram.addShaderBlock(*this);

	for(unsigned int i=0; i<maxVariables; i++)
		delete var_names[i];
	delete[] var_names;


	free(blockBuffer);
//	*/
  }

  ShaderBlockGL::~ShaderBlockGL()
  {
	glDeleteBuffers(1, &m_bufferObjectID);
  }

  bool ShaderBlockGL::setVariableValue(const string& variableName, const void* variableSource)
  {
	  map<string, unsigned int>::iterator iter = m_nameToIndex.find(variableName);
	  if(iter != m_nameToIndex.end())
	  {
		unsigned int& index = iter->second;
		glBindBuffer(GL_UNIFORM_BUFFER, m_bufferObjectID);
		glBufferSubData(GL_UNIFORM_BUFFER, m_variableOffset[index], m_variableByteSize[index], variableSource);
		return true;
	  }
	  return false;
  }

  bool ShaderBlockGL::setVariableValue(unsigned int locationID, unsigned int byteSize, const void* variableSource)
  {
		glBindBuffer(GL_UNIFORM_BUFFER, m_bufferObjectID);
		glBufferSubData(GL_UNIFORM_BUFFER, locationID, byteSize, variableSource);
		return true;
  }


  bool ShaderBlockGL::getVariableValue(const string& variableName, void* variableSource)
  {
	  map<string, unsigned int>::iterator iter = m_nameToIndex.find(variableName);
	  if(iter != m_nameToIndex.end())
	  {
		  unsigned int index = iter->second;
		  glGetBufferSubData( GL_UNIFORM_BUFFER, m_variableOffset[index], m_variableByteSize[index], variableSource );
		  return true;
	  }
	  return false;
  }

  bool ShaderBlockGL::getVariableValue(unsigned int locationID, unsigned int byteSize, void* variableSource)
  {
	glGetBufferSubData( GL_UNIFORM_BUFFER, locationID, byteSize, variableSource );
	return true;
  }

  bool ShaderBlockGL::getVariableLocation(const string& variableName, unsigned int& locationID) const
  {
	  map<string, unsigned int>::const_iterator iter = m_nameToIndex.find(variableName);
	  if(iter != m_nameToIndex.end())
	  {
		  locationID = iter->second;
		  return true;
	  }
	  return false;
  }



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


#include "main.h"

void InitShaders()
{
	auto& sceneShaderProgram	= shaderProgramList[SCENE_SHADER_ID];
	auto& screenShaderProgram	= shaderProgramList[SCREEN_SHADER_ID];
	auto& thresholdShaderProgram = shaderProgramList[THRESHOLD_SHADER_ID];
	auto& blurShaderProgram		= shaderProgramList[BLUR_SHADER_ID];
	auto& combineShaderProgram	= shaderProgramList[COMBINE_SHADER_ID];
	auto& skinShaderProgram	= shaderProgramList[SKIN_SHADER_ID];

	auto& shaderQuadBlock= shaderBlockList[QUAD_BLOCK_ID];

	{ //sceneShaderProgram
		vector<shared_ptr<ShaderGL>> vertexShaderList;
		vector<shared_ptr<ShaderGL>> fragmentShaderList;
		vector<string> attributeNameList;

		vertexShaderList.push_back(graphics.createShader(ShaderGL::VERTEX_SHADER, "../Data/Shader/sceneShader.vert"));
		fragmentShaderList.push_back(graphics.createShader(ShaderGL::FRAGMENT_SHADER, "../Data/Shader/sceneShader.frag"));

		attributeNameList.push_back("in_position");
		attributeNameList.push_back("in_normal");
		attributeNameList.push_back("in_textCoord");

		sceneShaderProgram = graphics.createShaderProgram("sceneShader", attributeNameList, vertexShaderList, fragmentShaderList);
		OutputShaderError(*sceneShaderProgram);

		sceneShaderProgram->bind();
		sceneShaderProgram->setVariableInteger(sceneShaderProgram->getVariableLocation("textureUnit1"), 0);
		sceneShaderInfo = InitShaderInfo(*sceneShaderProgram);
	}

	{//screenShaderProgram
		vector<shared_ptr<ShaderGL>> vertexShaderList;
		vector<shared_ptr<ShaderGL>> fragmentShaderList;
		vector<string> attributeNameList;

		vertexShaderList.push_back(graphics.createShader(ShaderGL::VERTEX_SHADER, "../Data/Shader/screenShader.vert"));
		fragmentShaderList.push_back(graphics.createShader(ShaderGL::FRAGMENT_SHADER, "../Data/Shader/screenShader.frag"));

		attributeNameList.push_back("in_position");

		screenShaderProgram = graphics.createShaderProgram("screenShader", attributeNameList, vertexShaderList, fragmentShaderList);
		OutputShaderError(*screenShaderProgram);

		screenShaderProgram->bind();
		screenShaderProgram->setVariableInteger(screenShaderProgram->getVariableLocation("textureUnit1"), 0);
	}

	{//thresholdShaderProgram
		vector<shared_ptr<ShaderGL>> vertexShaderList;
		vector<shared_ptr<ShaderGL>> fragmentShaderList;
		vector<string> attributeNameList;

		vertexShaderList.push_back(graphics.createShader(ShaderGL::VERTEX_SHADER, "../Data/Shader/screenShader.vert"));
		fragmentShaderList.push_back(graphics.createShader(ShaderGL::FRAGMENT_SHADER, "../Data/Shader/thresholdShader.frag"));

		attributeNameList.push_back("in_position");

		thresholdShaderProgram = graphics.createShaderProgram("thresholdShader", attributeNameList, vertexShaderList, fragmentShaderList);
		OutputShaderError(*thresholdShaderProgram);

		thresholdShaderProgram->bind();
		thresholdShaderProgram->setVariableInteger(thresholdShaderProgram->getVariableLocation("textureUnit1"), 0);
	}

	{ //blurShaderProgram
		vector<shared_ptr<ShaderGL>> vertexShaderList;
		vector<shared_ptr<ShaderGL>> fragmentShaderList;
		vector<string> attributeNameList;
		attributeNameList.push_back("in_position");

		vertexShaderList.push_back(graphics.createShader(ShaderGL::VERTEX_SHADER, "../Data/Shader/screenShader.vert"));
		fragmentShaderList.push_back(graphics.createShader(ShaderGL::FRAGMENT_SHADER, "../Data/Shader/blurShader.frag"));

		blurShaderProgram = graphics.createShaderProgram("blurShader", attributeNameList, vertexShaderList, fragmentShaderList);
		OutputShaderError(*blurShaderProgram);

		blurShaderProgram->bind();
		blurShaderProgram->setVariableInteger(blurShaderProgram->getVariableLocation("textureUnit1"), 0);
	}

	{//combineShaderProgram
		vector<shared_ptr<ShaderGL>> vertexShaderList;
		vector<shared_ptr<ShaderGL>> fragmentShaderList;
		vector<string> attributeNameList;
		attributeNameList.push_back("in_position");

		vertexShaderList.push_back(graphics.createShader(ShaderGL::VERTEX_SHADER, "../Data/Shader/screenShader.vert"));
		fragmentShaderList.push_back(graphics.createShader(ShaderGL::FRAGMENT_SHADER, "../Data/Shader/combineShader.frag"));

		combineShaderProgram = graphics.createShaderProgram("combineShader", attributeNameList, vertexShaderList, fragmentShaderList);
		OutputShaderError(*combineShaderProgram);

		combineShaderProgram->bind();
		combineShaderProgram->setVariableInteger(combineShaderProgram->getVariableLocation("textureUnit1"), 0);
		combineShaderProgram->setVariableInteger(combineShaderProgram->getVariableLocation("textureUnit2"), 1);
	}


	{ //skinShaderProgram
		vector<shared_ptr<ShaderGL>> vertexShaderList;
		vector<shared_ptr<ShaderGL>> fragmentShaderList;
		vector<string> attributeNameList;
		attributeNameList.push_back("in_position");
		attributeNameList.push_back("in_normal");
		attributeNameList.push_back("in_textCoord");

		vertexShaderList.push_back(graphics.createShader(ShaderGL::VERTEX_SHADER, "../Data/Shader/skinningShader.vert"));
		fragmentShaderList.push_back(graphics.createShader(ShaderGL::FRAGMENT_SHADER, "../Data/Shader/sceneShader.frag"));

		skinShaderProgram = graphics.createShaderProgram("skinShader", attributeNameList, vertexShaderList, fragmentShaderList);
		OutputShaderError(*skinShaderProgram);

		skinShaderProgram->bind();
		skinShaderProgram->setVariableInteger(skinShaderProgram->getVariableLocation("textureUnit1"), 0);
		skinShaderInfo = InitShaderInfo(*skinShaderProgram);
	}

	try
	{
		screenShaderProgram->bind();
		float quad[] = {1.0f/640.0f, 1.0f/480.0f};
		vector<ShaderVariableGL> variableList;
		variableList.push_back(ShaderVariableGL("pixelSize", sizeof(float)*2, quad));
		shaderQuadBlock.reset(new ShaderBlockGL(*screenShaderProgram, "QuadBlock", variableList));

		screenShaderProgram->addShaderBlock(*shaderQuadBlock);
		thresholdShaderProgram->addShaderBlock(*shaderQuadBlock);
		blurShaderProgram->addShaderBlock(*shaderQuadBlock);
		combineShaderProgram->addShaderBlock(*shaderQuadBlock);

	}
	catch(exception& ex)
	{
		MessageBox(NULL, ex.what(), "", 0);
	}

}

SceneShaderInfo InitShaderInfo(const ShaderProgramGL& program)
{
	return SceneShaderInfo(	program.getVariableLocation("modelMatrix"),
							program.getVariableLocation("projModelViewMatrix"),
							program.getVariableLocation("normalMatrix"),
							program.getVariableLocation("materialAmbient"),
							program.getVariableLocation("materialDiffuse"),
							program.getVariableLocation("materialSpecular"), 
							program.getVariableLocation("materialShininess"),
							program.getVariableLocation("lightAmbient"), 
							program.getVariableLocation("lightDiffuse"), 
							program.getVariableLocation("lightSpecular"),
							program.getVariableLocation("lightPosition"));
}

void OutputShaderError(const ShaderProgramGL& shaderProgram)
{
	#ifdef _WIN32
	int i=0;
	const char* error_msg;
	while( (error_msg =  shaderProgram.getErrorMsg(i)) != NULL  )
	{
		MessageBox(NULL, error_msg, "Error", 0);
		i++;
	}
	#endif
}

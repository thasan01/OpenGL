#include "main.h"

ShadowMapGL* shadowMap;

FrameBufferObjectGL*  fbo;
RenderBufferObjectGL* dbo;
RenderBufferObjectGL* rboList[RENDER_BUFFER_COUNT];
vector<glm::mat4> boneTransform;

SceneShaderInfo skinShaderInfo;
SceneShaderInfo sceneShaderInfo;
SceneShaderInfo shadowShaderInfo;
 
LightGL* ptrLight;

shared_ptr<ShaderProgramGL> shaderProgramList[SHADER_PROGRAM_COUNT];
shared_ptr<ShaderBlockGL> shaderBlockList[SHADER_BLOCK_COUNT];

shared_ptr<MeshGL> mesh;
shared_ptr<AnimatedMeshGL> animMesh;

unsigned int winWidth = 640, winHeight = 480;

unsigned int squareVBOIDList[1];
unsigned int squareVAOID;

float lightDirectionY = 355.2f;
float lightDirectionX = -45.0f;

//float lightDirectionX = -90.0f;
//float lightDirectionY = 0.0f;


void InitGL()
{
	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glReadBuffer(GL_BACK);
	glDrawBuffer(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClearDepth(1.0);

	glEnable(GL_TEXTURE_2D);
}

void InitSquare()
{
	unsigned int maxVertices = 4;
	Vertex3 vertList[] =  { -1.0f,1.0f,0.0, 
							-1.0f,-1.0f,0.0f,
							 1.0f,-1.0f,0.0f,
							 1.0f, 1.0f,0.0f };

	//create vbo(s)
	glGenBuffers(1, squareVBOIDList);
	glBindBuffer(GL_ARRAY_BUFFER, squareVBOIDList[0]);
	glBufferData(GL_ARRAY_BUFFER, maxVertices * 3 * sizeof(float), vertList, GL_STATIC_DRAW);

	//create voa
	glGenVertexArrays(1, &squareVAOID);
	glBindVertexArray(squareVAOID);

	glEnableVertexAttribArray(0);

	for(int i=1; i<10; i++)
		glDisableVertexAttribArray(i);

	glBindBuffer(GL_ARRAY_BUFFER, squareVBOIDList[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//unbind vao
	glBindVertexArray(0);
	//unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderSquare()
{
	glBindVertexArray(squareVAOID);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
}

void DeleteSquare()
{
	glDeleteVertexArrays(1, &squareVAOID);
	glDeleteBuffers(1, squareVBOIDList);
}

void RenderScene(bool renderToDepthBuffer)
{
	glm::mat4 shadowMatrix;

	glm::mat4 biasMatrix;
	biasMatrix[0][0]=0.5;biasMatrix[0][1]=0.0;biasMatrix[0][2]=0.0;biasMatrix[0][3]=0.0;
	biasMatrix[1][0]=0.0;biasMatrix[1][1]=0.5;biasMatrix[1][2]=0.0;biasMatrix[1][3]=0.0;
	biasMatrix[2][0]=0.0;biasMatrix[2][1]=0.0;biasMatrix[2][2]=0.5;biasMatrix[2][3]=0.0;
	biasMatrix[3][0]=0.5;biasMatrix[3][1]=0.5;biasMatrix[3][2]=0.5;biasMatrix[3][3]=1.0;

	const float* lightPosition = ptrLight->getPosition();
	glm::mat4 lightViewMatrix = glm::rotate(glm::mat4(), -lightDirectionX, glm::vec3(1,0,0));
	lightViewMatrix = glm::rotate(lightViewMatrix, -lightDirectionY, glm::vec3(0,1,0));
	lightViewMatrix = glm::translate(lightViewMatrix, glm::vec3(-lightPosition[0],-lightPosition[1],-lightPosition[2]));

	//Render Ground
	auto activeShaderProgram = renderToDepthBuffer ? shaderProgramList[SHADOW_SHADER_ID] : shaderProgramList[SCENE_SHADER_ID];
	SceneShaderInfo* activeShaderInfo = renderToDepthBuffer ? &shadowShaderInfo : &sceneShaderInfo;

	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0,-20,0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(500,1,500));
	modelMatrix = glm::rotate(modelMatrix, -90.0f, glm::vec3(1.0f,0.0f,0.0f));		  

	glm::mat4 viewMatrix = renderToDepthBuffer ? lightViewMatrix : glm::lookAt(glm::vec3(-100,50,150), glm::vec3(0,0,0), glm::vec3(0,1,0));
	glm::mat4 projectionMatrix = glm::perspective<float>(45.0f, float(winWidth)/float(winHeight), 1.0f, 1000.0f);
	shadowMatrix = biasMatrix * projectionMatrix* lightViewMatrix * modelMatrix;
	glm::mat4 projModelViewMatrix = projectionMatrix * viewMatrix * modelMatrix;

	activeShaderProgram->bind();
	if(renderToDepthBuffer)
	{
		GLuint staticIndex   = glGetSubroutineIndex(activeShaderProgram->getProgramID(), GL_VERTEX_SHADER,"transformStaticVertex" );
		GLuint animatedIndex = glGetSubroutineIndex(activeShaderProgram->getProgramID(), GL_VERTEX_SHADER, "transformAnimatedVertex");
		glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &staticIndex);
	}
	else
	{
		activeShaderProgram->setVariableMatrix4f(activeShaderProgram->getVariableLocation("lightProjModelViewMatrix"), glm::value_ptr(shadowMatrix));

		glm::mat3 normalMatrix =  glm::inverseTranspose(glm::mat3(modelMatrix));
		activeShaderProgram->setVariableMatrix3f(activeShaderInfo->m_normalMatrixLocation, glm::value_ptr(normalMatrix));
		ptrLight->bind(*activeShaderInfo, *activeShaderProgram);
	}

	activeShaderProgram->setVariableMatrix4f(activeShaderInfo->m_modelMatrixLocation, glm::value_ptr(modelMatrix));
	activeShaderProgram->setVariableMatrix4f(activeShaderInfo->m_projModelViewMatrixLocation, glm::value_ptr(projModelViewMatrix));

	mesh->render(*activeShaderInfo, activeShaderProgram);


	//Render spinning cube
	static float theta= 0.0f;
	static float posY = 0.0f;
	static float velY = 0.10f;


	if(renderToDepthBuffer)
	{
		theta += 0.5f;

		posY += velY;
		if(fabs(posY) > 50 )
			velY *= -1.0f;
	}

	modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0,posY+50,50));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(30,30,30));
	modelMatrix = glm::rotate(modelMatrix, theta, glm::vec3(0.0f,1.0f,0.0f));		  
	modelMatrix = glm::rotate(modelMatrix, -90.0f, glm::vec3(1.0f,0.0f,0.0f));		  

	shadowMatrix = biasMatrix * projectionMatrix* lightViewMatrix * modelMatrix;
	projModelViewMatrix = projectionMatrix * viewMatrix * modelMatrix;

	activeShaderProgram->bind();
	if(renderToDepthBuffer)
	{
		GLuint staticIndex   = glGetSubroutineIndex(activeShaderProgram->getProgramID(), GL_VERTEX_SHADER,"transformStaticVertex" );
		glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &staticIndex);
	}
	else
	{
		activeShaderProgram->setVariableMatrix4f(activeShaderProgram->getVariableLocation("lightProjModelViewMatrix"), glm::value_ptr(shadowMatrix));

		glm::mat3 normalMatrix =  glm::inverseTranspose(glm::mat3(modelMatrix));
		activeShaderProgram->setVariableMatrix3f(activeShaderInfo->m_normalMatrixLocation, glm::value_ptr(normalMatrix));
		ptrLight->bind(*activeShaderInfo, *activeShaderProgram);
	}

	activeShaderProgram->setVariableMatrix4f(activeShaderInfo->m_modelMatrixLocation, glm::value_ptr(modelMatrix));
	activeShaderProgram->setVariableMatrix4f(activeShaderInfo->m_projModelViewMatrixLocation, glm::value_ptr(projModelViewMatrix));

	mesh->render(*activeShaderInfo, activeShaderProgram);


	//Render Animated Mesh
	activeShaderProgram = renderToDepthBuffer ? shaderProgramList[SHADOW_SHADER_ID] : shaderProgramList[SKIN_SHADER_ID];
	activeShaderInfo = renderToDepthBuffer ? &shadowShaderInfo : &skinShaderInfo;

	modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0,-10,0));
	modelMatrix = glm::rotate(modelMatrix, -90.0f, glm::vec3(1.0f,0.0f,0.0f));

	activeShaderProgram->bind();
	shadowMatrix = biasMatrix * projectionMatrix* lightViewMatrix * modelMatrix;
	projModelViewMatrix = projectionMatrix * viewMatrix * modelMatrix;

	if(renderToDepthBuffer)
	{
		GLuint animatedIndex = glGetSubroutineIndex(activeShaderProgram->getProgramID(), GL_VERTEX_SHADER, "transformAnimatedVertex");
		glUniformSubroutinesuiv( GL_VERTEX_SHADER, 1, &animatedIndex);
	}
	else
	{
		activeShaderProgram->setVariableMatrix4f(activeShaderProgram->getVariableLocation("lightProjModelViewMatrix"), glm::value_ptr(shadowMatrix));

		glm::mat3 normalMatrix =  glm::inverseTranspose(glm::mat3(modelMatrix));
		activeShaderProgram->setVariableMatrix3f(activeShaderInfo->m_normalMatrixLocation, glm::value_ptr(normalMatrix));
		ptrLight->bind(*activeShaderInfo, *activeShaderProgram);
	}

	activeShaderProgram->setVariableMatrix4f(activeShaderInfo->m_modelMatrixLocation, glm::value_ptr(modelMatrix));
	activeShaderProgram->setVariableMatrix4f(activeShaderInfo->m_projModelViewMatrixLocation, glm::value_ptr(projModelViewMatrix));

	animMesh->render(*activeShaderInfo, activeShaderProgram);
}

class MainScreen : public ScreenSDLGL
{
  public:
	  MainScreen(SDL_Window& window) : ScreenSDLGL(window) {}

	  virtual void onRender()
	  {
		  bool enableBloom = false;
		  bool enableShadow = true;
		  auto& sceneShaderProgram		= shaderProgramList[SCENE_SHADER_ID];
		  auto& screenShaderProgram		= shaderProgramList[SCREEN_SHADER_ID];
		  auto& thresholdShaderProgram	= shaderProgramList[THRESHOLD_SHADER_ID];
		  auto& blurShaderProgram		= shaderProgramList[BLUR_SHADER_ID];
		  auto& combineShaderProgram	= shaderProgramList[COMBINE_SHADER_ID];
		  auto& skinShaderProgram		= shaderProgramList[SKIN_SHADER_ID];
		  auto& shaderQuadBlock			= shaderBlockList[QUAD_BLOCK_ID];
		  auto& shadowShaderProgram	= shaderProgramList[SHADOW_SHADER_ID];

		  if(enableShadow)
		  {
			  shadowMap->beginFirstPass();
			  RenderScene(true);
			  shadowMap->endFirstPass(winWidth, winHeight);
		  }

		  //Render to Scene texture
		  fbo->bind();	
		  fbo->swap(*rboList[0]);

		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		  

		  sceneShaderProgram->bind();
		  shadowMap->beginSecondPass(GL_TEXTURE1);
		  RenderScene(false);
		  shadowMap->endSecondPass();

		  bool turn = true;
		  if(enableBloom)
		  {
			  glDisable(GL_DEPTH_TEST);

			  //Extract scene intensity
			  fbo->swap(*rboList[1]);
			  thresholdShaderProgram->bind();
			  thresholdShaderProgram->bindShaderBlock(*shaderQuadBlock);

			  glBindTexture(GL_TEXTURE_2D, rboList[0]->getBufferID());
			  RenderSquare();
			  thresholdShaderProgram->unbind();

			  //Blur scene
			  glActiveTexture(GL_TEXTURE0);

			  blurShaderProgram->bind();
			  blurShaderProgram->bindShaderBlock(*shaderQuadBlock);
			  for(int i=0; i<10; i++)
			  {
				  fbo->swap(turn ? *rboList[2] : *rboList[3]);

				  if(i==0)
					  glBindTexture(GL_TEXTURE_2D, rboList[1]->getBufferID());
				  else
					  glBindTexture(GL_TEXTURE_2D, turn ? rboList[3]->getBufferID() : rboList[2]->getBufferID());

				  blurShaderProgram->setVariableInteger(blurShaderProgram->getVariableLocation("isVertical"), int(turn));
				  RenderSquare();
				  turn = !turn;
			  }
			  blurShaderProgram->unbind();
		  }
		  fbo->unbind();

		  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		  if(enableBloom)
		  {
			  combineShaderProgram->bind();
			  combineShaderProgram->bindShaderBlock(*shaderQuadBlock);

			  glActiveTexture(GL_TEXTURE0);
			  glBindTexture(GL_TEXTURE_2D, rboList[0]->getBufferID());
			  glActiveTexture(GL_TEXTURE1);
			  glBindTexture(GL_TEXTURE_2D, (turn ? rboList[2] : rboList[3])->getBufferID());
		  }
		  else
		  {
			  screenShaderProgram->bind();
			  screenShaderProgram->bindShaderBlock(*shaderQuadBlock);
			  glActiveTexture(GL_TEXTURE0);
			  glBindTexture(GL_TEXTURE_2D, rboList[0]->getBufferID() );
		  }

		  RenderSquare();
		  combineShaderProgram->unbind();
	  }

};


void PostProcessMeshFileData(MeshFileData& mfd)
{
	for(unsigned int i=0; i<mfd.m_materialData.size(); i++)
	{//make sure all the materials have unique names
		stringstream ss;
		ss << mfd.m_materialData[i].m_name << "_" << i;
		mfd.m_materialData[i].m_name = ss.str();

		mfd.m_materialData[i].m_ambient.m_c[0]=0.5f;
		mfd.m_materialData[i].m_ambient.m_c[1]=0.5f;
		mfd.m_materialData[i].m_ambient.m_c[2]=0.5f;
		mfd.m_materialData[i].m_ambient.m_c[3]=1.0f;

		mfd.m_materialData[i].m_diffuse.m_c[0]=0.5f;
		mfd.m_materialData[i].m_diffuse.m_c[1]=0.5f;
		mfd.m_materialData[i].m_diffuse.m_c[2]=0.5f;
		mfd.m_materialData[i].m_diffuse.m_c[3]=1.0f;

		mfd.m_materialData[i].m_specular.m_c[0]=0.5f;
		mfd.m_materialData[i].m_specular.m_c[1]=0.5f;
		mfd.m_materialData[i].m_specular.m_c[2]=0.5f;
		mfd.m_materialData[i].m_specular.m_c[3]=1.0f;

		mfd.m_materialData[i].m_shininess=1;
	}
}



int main( int argc, char *argv[] )
{
	fileLogger.open("game.log");
	WindowSDL window;
	window.create("SDL2/OpenGL Demo", 30, 30, winWidth, winHeight);

	MainScreen screen(*window.getSDLWindow());
	InitGL();
	InitSquare();


	#ifdef _WIN32
		stringstream ss;
		ss << "major: " << screen.getMajorVersion() << " minor: " << screen.getMinorVersion() << endl;
		OutputDebugString(ss.str().data());
	#endif

	InitShaders();

	float ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
	float diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
	float specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float position[] = { 0.0f, 500.0f, 500.0f};
	ptrLight = new LightGL(ambient, diffuse, specular, position);

	Skeleton* ptrSkeleton = NULL; 
	try
	{
		AssimpMeshFileReader reader;
		{
			MeshFileData mfd;
			reader.load("boblampclean.md5mesh", mfd);
			PostProcessMeshFileData(mfd);

			animMesh.reset( new AnimatedMeshGL(mfd.m_meshData[0], mfd.m_materialData) );
			ptrSkeleton = new Skeleton(mfd.m_skeleton);
		}
		{
			MeshFileData mfd;
			reader.load("cube.3ds", mfd);		
			PostProcessMeshFileData(mfd);
			mesh = graphics.createMesh("cube", mfd);
		}
	}
	catch(const exception& ex)
	{
		MessageBox(NULL, ex.what(), "Unhandled Exception", 0);
		fileLogger.close();
		return false;
	}

	AnimationComponent animComp(0,0);

	//Main loop flag 
	bool bQuit = false; 
	SDL_Event event;

	boneTransform.resize(animMesh->getBoneCount());
	auto& skinShaderProgram	= shaderProgramList[SKIN_SHADER_ID];
	unsigned int initBoneMatrixLocation = skinShaderProgram->getVariableLocation("boneMatrix[0]");
	unsigned int initShadowBoneMatrixLocation = shaderProgramList[SHADOW_SHADER_ID]->getVariableLocation("boneMatrix[0]");

	skinShaderProgram->bind();
	for(unsigned int i=0, max = animMesh->getBoneCount(); i<max; i++)
		skinShaderProgram->setVariableMatrix4f(initBoneMatrixLocation + i, glm::value_ptr(boneTransform[i]));			

	shaderProgramList[SHADOW_SHADER_ID]->bind();
	for(unsigned int i=0, max = animMesh->getBoneCount(); i<max; i++)
		shaderProgramList[SHADOW_SHADER_ID]->setVariableMatrix4f(initShadowBoneMatrixLocation + i, glm::value_ptr(boneTransform[i]));			


	unsigned int initMilliSecTime = SDL_GetTicks();
	fbo = new FrameBufferObjectGL();
	dbo = new RenderBufferObjectGL(winWidth, winHeight, true);
	for(unsigned int i=0; i<RENDER_BUFFER_COUNT; i++)
		rboList[i] = new RenderBufferObjectGL(winWidth, winHeight, false);

	fbo->attach(*rboList[0]);
	fbo->attach(*dbo);

	shadowMap = new ShadowMapGL(winWidth*4, winHeight*4);

	while(!bQuit) 
	{	
		while( SDL_PollEvent( &event ) != 0 ) 
		{ 

			switch(event.type)
			{
				case SDL_QUIT:
					bQuit = true;
					break;

				case SDL_KEYDOWN:
					window.onKeyPress(event.key.keysym.sym);
					break;

				case SDL_KEYUP:
					window.onKeyRelease(event.key.keysym.sym);
					break;

				default:;
			}
		}

		unsigned int currentMilliSecTime = SDL_GetTicks();
		double timeEllasped = double(currentMilliSecTime - initMilliSecTime)/ 1000.0f;
		animMesh->getBoneTransformation(timeEllasped, *ptrSkeleton, animComp, boneTransform);

		skinShaderProgram->bind();
		for(unsigned int i=0, max = animMesh->getBoneCount(); i<max; i++)
			skinShaderProgram->setVariableMatrix4f(initBoneMatrixLocation + i, glm::value_ptr(boneTransform[i]));			

		shaderProgramList[SHADOW_SHADER_ID]->bind();
		for(unsigned int i=0, max = animMesh->getBoneCount(); i<max; i++)
			shaderProgramList[SHADOW_SHADER_ID]->setVariableMatrix4f(initShadowBoneMatrixLocation + i, glm::value_ptr(boneTransform[i]));			

		screen.render(*window.getSDLWindow());
	}

	for(unsigned int i=0; i<SHADER_PROGRAM_COUNT; i++)
	{
		if(shaderProgramList[i])
			graphics.releaseShaderProgram(shaderProgramList[i]);
	}

	DeleteSquare();


	if(mesh)
		graphics.releaseMesh(mesh);


	delete shadowMap;
	delete fbo;
	delete dbo;
	for(unsigned int i=0; i<RENDER_BUFFER_COUNT; i++)
		delete rboList[i];

	delete ptrLight;
	delete ptrSkeleton;

	animMesh.reset();

	SDL_Quit();
	fileLogger.close();
	return 0;
}

#include "main.h"

FrameBufferObjectGL*  fbo;
RenderBufferObjectGL* dbo;
RenderBufferObjectGL* rboList[RENDER_BUFFER_COUNT];

SceneShaderInfo skinShaderInfo;
SceneShaderInfo sceneShaderInfo;

LightGL* ptrLight;

shared_ptr<ShaderProgramGL> shaderProgramList[SHADER_PROGRAM_COUNT];
shared_ptr<ShaderBlockGL> shaderBlockList[SHADER_BLOCK_COUNT];

shared_ptr<MeshGL> mesh;
shared_ptr<AnimatedMeshGL> animMesh;

unsigned int winWidth = 640, winHeight = 480;

unsigned int squareVBOIDList[1];
unsigned int squareVAOID;


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

class MainScreen : public ScreenSDLGL
{
  public:
	  MainScreen(SDL_Window& window) : ScreenSDLGL(window) {}

	  virtual void onRender()
	  {
		  bool renderBloom = true;
		  auto& sceneShaderProgram		= shaderProgramList[SCENE_SHADER_ID];
		  auto& screenShaderProgram		= shaderProgramList[SCREEN_SHADER_ID];
		  auto& thresholdShaderProgram	= shaderProgramList[THRESHOLD_SHADER_ID];
		  auto& blurShaderProgram		= shaderProgramList[BLUR_SHADER_ID];
		  auto& combineShaderProgram	= shaderProgramList[COMBINE_SHADER_ID];
		  auto& skinShaderProgram		= shaderProgramList[SKIN_SHADER_ID];
		  auto& shaderQuadBlock			= shaderBlockList[QUAD_BLOCK_ID];


		  //Render to Scene texture
		  fbo->bind();	
		  fbo->swap(*rboList[0]);

		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		  glEnable(GL_DEPTH_TEST);

		  float pixelSize[2] = { (1.0f/winWidth), (1.0f/winHeight) };
		  shaderQuadBlock->setVariableValue("pixelSize", pixelSize);


		  glm::mat4 modelMatrix;
		  modelMatrix = glm::translate(modelMatrix, glm::vec3(0,0,-50));
		  modelMatrix = glm::scale(modelMatrix, glm::vec3(100,100,1));
		  modelMatrix = glm::rotate(modelMatrix, -90.0f, glm::vec3(1.0f,0.0f,0.0f));		  

		  glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0,0,100), glm::vec3(0,0,0), glm::vec3(0,1,0));
		  glm::mat4 projectionMatrix = glm::perspective<float>(45.0f, float(winWidth)/float(winHeight), 1.0f, 1000.0f);
		  glm::mat4 projModelViewMatrix = projectionMatrix * viewMatrix * modelMatrix;
		  glm::mat3 normalMatrix =  glm::inverseTranspose(glm::mat3(modelMatrix));


		  sceneShaderProgram->bind();
		  sceneShaderProgram->setVariableMatrix4f(sceneShaderInfo.m_modelMatrixLocation, glm::value_ptr(modelMatrix));
		  sceneShaderProgram->setVariableMatrix4f(sceneShaderInfo.m_projModelViewMatrixLocation, glm::value_ptr(projModelViewMatrix));
		  sceneShaderProgram->setVariableMatrix3f(sceneShaderInfo.m_normalMatrixLocation, glm::value_ptr(normalMatrix));

		  ptrLight->bind(sceneShaderInfo, *sceneShaderProgram);
		  mesh->render(sceneShaderInfo, sceneShaderProgram);

		  modelMatrix = glm::mat4();
		  modelMatrix = glm::translate(modelMatrix, glm::vec3(0,-20,0));
		  modelMatrix = glm::translate(modelMatrix, glm::vec3(0,0,0));
		  modelMatrix = glm::rotate(modelMatrix, -90.0f, glm::vec3(1.0f,0.0f,0.0f));

		  projectionMatrix = glm::perspective<float>(45.0f, float(winWidth)/float(winHeight), 1.0f, 1000.0f);
		  projModelViewMatrix = projectionMatrix * viewMatrix * modelMatrix;

		  skinShaderProgram->bind();
		  skinShaderProgram->setVariableMatrix4f(skinShaderInfo.m_modelMatrixLocation, glm::value_ptr(modelMatrix));
		  skinShaderProgram->setVariableMatrix4f(skinShaderInfo.m_projModelViewMatrixLocation, glm::value_ptr(projModelViewMatrix));
		  ptrLight->bind(skinShaderInfo, *skinShaderProgram);
		  animMesh->render(skinShaderInfo, skinShaderProgram);

		bool turn = true;
		if(renderBloom)
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

		if(renderBloom)
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
	float position[] = { 0.0f, 500.0f, 500.0f };
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

	vector<glm::mat4> boneTransform(animMesh->getBoneCount());
	auto& skinShaderProgram	= shaderProgramList[SKIN_SHADER_ID];
	unsigned int initBoneMatrixLocation = skinShaderProgram->getVariableLocation("boneMatrix[0]");
	unsigned int initMilliSecTime = SDL_GetTicks();

	fbo = new FrameBufferObjectGL();
	dbo = new RenderBufferObjectGL(winWidth, winHeight, true);
	for(unsigned int i=0; i<RENDER_BUFFER_COUNT; i++)
		rboList[i] = new RenderBufferObjectGL(winWidth, winHeight, false);

	fbo->attach(*rboList[0]);
	fbo->attach(*dbo);

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

		skinShaderProgram->bind();
		animMesh->getBoneTransformation(timeEllasped, *ptrSkeleton, animComp, boneTransform);
		for(unsigned int i=0, max = animMesh->getBoneCount(); i<max; i++)
			skinShaderProgram->setVariableMatrix4f(initBoneMatrixLocation + i, glm::value_ptr(boneTransform[i]));			

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

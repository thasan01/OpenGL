#include "Globals.h"

SceneShaderInfo skinShaderInfo;
SceneShaderInfo sceneShaderInfo;

LightGL* ptrLight;
shared_ptr<ShaderProgramGL> sceneShaderProgram;
shared_ptr<ShaderProgramGL> skinShaderProgram;
shared_ptr<MeshGL> mesh;
shared_ptr<AnimatedMeshGL> animMesh;

unsigned int winWidth = 640, winHeight = 480;

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


class MainScreen : public ScreenSDLGL
{
  public:
	  MainScreen(SDL_Window& window) : ScreenSDLGL(window) {}

	  virtual void onRender()
	  {
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		  glm::mat4 modelMatrix;
		  modelMatrix = glm::translate(modelMatrix, glm::vec3(0,0,-50));
		  modelMatrix = glm::scale(modelMatrix, glm::vec3(100,100,1));
		  modelMatrix = glm::rotate(modelMatrix, -90.0f, glm::vec3(1.0f,0.0f,0.0f));
		  
		  glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0,0,100), glm::vec3(0,0,0), glm::vec3(0,1,0));
		  glm::mat4 projectionMatrix = glm::perspective<float>(45.0f, float(winWidth)/float(winHeight), 1.0f, 1000.0f);
		  glm::mat4 projModelViewMatrix = projectionMatrix * viewMatrix * modelMatrix;

		  sceneShaderProgram->bind();
		  sceneShaderProgram->setVariableMatrix4f(sceneShaderInfo.m_modelMatrixLocation, glm::value_ptr(modelMatrix));
		  sceneShaderProgram->setVariableMatrix4f(sceneShaderInfo.m_projModelViewMatrixLocation, glm::value_ptr(projModelViewMatrix));
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
	  }

};

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

	#ifdef _WIN32 //
		stringstream ss;
		ss << "major: " << screen.getMajorVersion() << " minor: " << screen.getMinorVersion() << endl;
		OutputDebugString(ss.str().data());
	#endif

	{
		vector<shared_ptr<ShaderGL>> vertexShaderList;
		vector<shared_ptr<ShaderGL>> fragmentShaderList;
		vector<string> attributeNameList;

		vertexShaderList.push_back(graphics.createShader(ShaderGL::VERTEX_SHADER, "../Data/Shader/sceneShader.vert"));
		fragmentShaderList.push_back(graphics.createShader(ShaderGL::FRAGMENT_SHADER, "../Data/Shader/sceneShader.frag"));

		attributeNameList.push_back("in_position");
		attributeNameList.push_back("in_normal");
		attributeNameList.push_back("in_textCoord");

		sceneShaderProgram = graphics.createShaderProgram("sceneShader", attributeNameList, vertexShaderList, fragmentShaderList);
		sceneShaderProgram->bind();
		sceneShaderProgram->setVariableInteger(sceneShaderProgram->getVariableLocation("textureUnit1"), 0);
		sceneShaderInfo = InitShaderInfo(*sceneShaderProgram);
	}

	{
		vector<shared_ptr<ShaderGL>> vertexShaderList;
		vector<shared_ptr<ShaderGL>> fragmentShaderList;
		vector<string> attributeNameList;

		vertexShaderList.push_back(graphics.createShader(ShaderGL::VERTEX_SHADER, "../Data/Shader/skinningShader.vert"));
		fragmentShaderList.push_back(graphics.createShader(ShaderGL::FRAGMENT_SHADER, "../Data/Shader/sceneShader.frag"));

		attributeNameList.push_back("in_position");
		attributeNameList.push_back("in_normal");
		attributeNameList.push_back("in_textCoord");

		skinShaderProgram = graphics.createShaderProgram("skinShader", attributeNameList, vertexShaderList, fragmentShaderList);
		skinShaderProgram->bind();
		skinShaderProgram->setVariableInteger(skinShaderProgram->getVariableLocation("textureUnit1"), 0);
		skinShaderInfo = InitShaderInfo(*skinShaderProgram);
	}

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


	AnimationComponent animComp;
	animComp.m_animationIndex=0;
	animComp.m_startFrameIndex=0;
	animComp.m_endFrameIndex=0;
	animComp.m_remainingTime=0.0;

	//Main loop flag 
	bool bQuit = false; 
	SDL_Event event;

	vector<glm::mat4> boneTransform(animMesh->getBoneCount());
	unsigned int initBoneMatrixLocation = skinShaderProgram->getVariableLocation("boneMatrix[0]");
	unsigned int initMilliSecTime = SDL_GetTicks();

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
		for(unsigned int i=0, max = animMesh->getBoneCount(); i<max; i++)
			skinShaderProgram->setVariableMatrix4f(initBoneMatrixLocation + i, glm::value_ptr(boneTransform[i]));			

		screen.render(*window.getSDLWindow());
	}

	if(sceneShaderProgram)
		graphics.releaseShaderProgram(sceneShaderProgram);

	if(mesh)
		graphics.releaseMesh(mesh);


	delete ptrLight;
	//delete ptrSkeleton;
	animMesh.reset();


	SDL_Quit();
	fileLogger.close();
	return 0;
}

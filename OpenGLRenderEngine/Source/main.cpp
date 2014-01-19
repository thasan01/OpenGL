#include "Globals.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

SceneShaderInfo shaderInfo;
shared_ptr<ShaderProgramGL> sceneShaderProgram;
shared_ptr<MeshGL> mesh;

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

		  static float theta = 0.0f;
		  glm::mat4 modelMatrix = glm::rotate(glm::mat4(), theta, glm::vec3(0.0f,1.0f,0.0f));
		  glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,0), glm::vec3(0,1,0));
		  glm::mat4 projectionMatrix = glm::perspective<float>(45.0f, float(winWidth)/float(winHeight), 1.0f, 100.0f);
		  glm::mat4 projModelViewMatrix = projectionMatrix * viewMatrix * modelMatrix;

		  sceneShaderProgram->bind();
		  sceneShaderProgram->setVariableMatrix4f(shaderInfo.m_modelMatrixLocation, glm::value_ptr(modelMatrix));
		  sceneShaderProgram->setVariableMatrix4f(shaderInfo.m_projModelViewMatrixLocation, glm::value_ptr(projModelViewMatrix));
		  theta+=0.5f;

		  mesh->render(shaderInfo, sceneShaderProgram);
	  }

};

int main( int argc, char *argv[] )
{
	WindowSDL window;
	window.create("SDL2/OpenGL Demo", 30, 30, winWidth, winHeight);

	MainScreen screen(*window.getSDLWindow());
	InitGL();

	stringstream ss;
	ss << "major: " << screen.getMajorVersion() << " minor: " << screen.getMinorVersion() << endl;
	OutputDebugString(ss.str().data());

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
	}

	shaderInfo = SceneShaderInfo(	sceneShaderProgram->getVariableLocation("modelMatrix"),
									sceneShaderProgram->getVariableLocation("projModelViewMatrix"),
									sceneShaderProgram->getVariableLocation("normalMatrix"),
									sceneShaderProgram->getVariableLocation("materialAmbient"),
									sceneShaderProgram->getVariableLocation("materialDiffuse"),
									sceneShaderProgram->getVariableLocation("materialSpecular"), 
									sceneShaderProgram->getVariableLocation("materialShininess"),
									sceneShaderProgram->getVariableLocation("lightAmbient"), 
									sceneShaderProgram->getVariableLocation("lightDiffuse"), 
									sceneShaderProgram->getVariableLocation("lightSpecular"),
									sceneShaderProgram->getVariableLocation("lightPosition"));



	float ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
	float diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
	float specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float position[] = { 0.0f, 1000.0f, 1000.0f };
	LightGL light = LightGL(ambient, diffuse, specular, position);
	light.bind(shaderInfo, *sceneShaderProgram);

	AssimpMeshFileReader reader;
	MeshFileData mfd;
	glm::mat4 originMatrix = glm::rotate(glm::mat4(), -90.0f, glm::vec3(1.0f,0.0f,0.0f));
	reader.load("cube.3ds", mfd, glm::value_ptr(originMatrix));

	for(unsigned int i=0; i<mfd.m_materialData.size(); i++)
	{
		mfd.m_materialData[i].m_name = "cube_"+mfd.m_materialData[i].m_name;
		mfd.m_materialData[i].m_shininess = 1.0f;
	}

	mesh = graphics.createMesh("cube", mfd);

	//Main loop flag 
	bool bQuit = false; 
	SDL_Event event;

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
		screen.render(*window.getSDLWindow());
	}


	SDL_Quit();
	return 0;
}

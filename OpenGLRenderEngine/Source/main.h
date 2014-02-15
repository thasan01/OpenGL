#pragma once
#include "Globals.h"

#define SHADER_PROGRAM_COUNT 6
#define SCENE_SHADER_ID		0
#define SCREEN_SHADER_ID	1
#define THRESHOLD_SHADER_ID	2
#define BLUR_SHADER_ID		3
#define COMBINE_SHADER_ID	4
#define SKIN_SHADER_ID		5

#define SHADER_BLOCK_COUNT	1
#define QUAD_BLOCK_ID		0

#define RENDER_BUFFER_COUNT 5

extern FrameBufferObjectGL*  fbo;
extern RenderBufferObjectGL* dbo;
extern RenderBufferObjectGL* rboList[RENDER_BUFFER_COUNT];

extern SceneShaderInfo skinShaderInfo;
extern SceneShaderInfo sceneShaderInfo;

extern LightGL* ptrLight;

extern shared_ptr<ShaderProgramGL> shaderProgramList[SHADER_PROGRAM_COUNT];
extern shared_ptr<ShaderBlockGL> shaderBlockList[SHADER_BLOCK_COUNT];

extern shared_ptr<MeshGL> mesh;
extern shared_ptr<AnimatedMeshGL> animMesh;

extern unsigned int winWidth;
extern unsigned int winHeight;

extern unsigned int squareVBOIDList[1];
extern unsigned int squareVAOID;


//Functions defined in InitShader.cpp 
void InitShaders();
SceneShaderInfo InitShaderInfo(const ShaderProgramGL& program);
void OutputShaderError(const ShaderProgramGL& shaderProgram);

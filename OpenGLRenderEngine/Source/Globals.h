#pragma once

#include <windows.h>

#include "ResourceLibrary.h"
#include "Platform/WindowSDL.h"
#include "Platform/ScreenSDLGL.h"
#include "Graphics/GraphicsGL.h"
#include "File/FileReaderAssimp.h"
#include "Debug/FileLogger.h"
#include "Animation\AnimationSystem.h"

extern GraphicsGL graphics;
extern FileLogger fileLogger;

glm::vec3 BlendVec3(const glm::vec3& start, const glm::vec3& end, float t);

#include "Globals.h"

GraphicsGL graphics;
FileLogger fileLogger;

  glm::vec3 BlendVec3(const glm::vec3& start, const glm::vec3& end, float t)
  {
	  float it = 1-t;
	  return glm::vec3(start.x * it, start.y * it, start.z * it) + glm::vec3(end.x * t, end.y * t, end.z * t);
  }

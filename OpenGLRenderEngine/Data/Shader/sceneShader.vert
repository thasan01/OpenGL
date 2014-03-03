#version 330 core
//shader.vert

#ifdef USE_CORE
#else

  uniform mat4 modelMatrix;
  uniform mat4 projModelViewMatrix;	// (projection x view x model) matrix
  uniform mat3 normalMatrix;

  uniform mat4 lightProjModelViewMatrix;


  in vec3 in_position;
  in vec3 in_normal;
  in vec2 in_textCoord;

  out Data
  {
	vec2 textCoord;
	vec3 position;
	vec3 normal;
	vec4 lightVertexPosition; //position of vertex in light space.
  } 
  DataOut; 

#endif


void main() 
{            
  vec4 vertex4 = vec4(in_position, 1.0);

  DataOut.position = vec3(modelMatrix * vertex4);
  DataOut.normal = normalize(normalMatrix * in_normal);
  DataOut.textCoord = in_textCoord;
  
  DataOut.lightVertexPosition = lightProjModelViewMatrix * vertex4;


  gl_Position = projModelViewMatrix * vertex4;
}
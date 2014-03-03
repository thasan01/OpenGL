#version 330 core
//skinningShader.vert

  const int MAX_BONES = 100;

  uniform mat4 modelMatrix;
  uniform mat4 projModelViewMatrix;	// (projection x view x model) matrix
  uniform mat3 normalMatrix;
  uniform mat4 lightProjModelViewMatrix;

  uniform mat4 boneMatrix[MAX_BONES];


  in vec3 in_position;
  in vec3 in_normal;
  in vec2 in_textCoord;

  in ivec4 in_boneIDs;
  in vec4  in_weights;

  out Data
  {
	vec2 textCoord;
	vec3 position;
	vec3 normal;
	vec4 lightVertexPosition; //position of vertex in light space.
  } 
  DataOut; 


void main() 
{            

  mat4 boneTransform = boneMatrix[in_boneIDs[0]] * in_weights[0];
  boneTransform += boneMatrix[in_boneIDs[1]] * in_weights[1];
  boneTransform += boneMatrix[in_boneIDs[2]] * in_weights[2];
  boneTransform += boneMatrix[in_boneIDs[3]] * in_weights[3];

  vec4 vertex4 = boneTransform * vec4(in_position, 1.0);  
   
  DataOut.position = vec3(boneTransform * modelMatrix * vertex4);
  DataOut.normal = normalize( mat3(boneTransform) * normalMatrix * in_normal);
  //DataOut.normal = normalize( transpose(inverse(mat3(modelMatrix))) * in_normal);
  DataOut.textCoord = in_textCoord;

  DataOut.lightVertexPosition = lightProjModelViewMatrix * vertex4;


  gl_Position = projModelViewMatrix * vertex4;

}
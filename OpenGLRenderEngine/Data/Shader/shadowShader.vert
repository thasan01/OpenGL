#version 400 core
//shadowShader.vert

  const int MAX_BONES = 100;
  uniform mat4 projModelViewMatrix;	// (projection x view x model) matrix
  uniform mat4 boneMatrix[MAX_BONES];

  in vec3 in_position;
  in vec3 in_normal;
  in vec2 in_textCoord;

  in ivec4 in_boneIDs;
  in vec4  in_weights;

  subroutine vec4 VertexTransformType(in vec4 v);  
  subroutine uniform VertexTransformType transformType;

  
  subroutine( VertexTransformType )
  vec4 transformStaticVertex(in vec4 v)
  {
    return projModelViewMatrix * v;
  }

  subroutine( VertexTransformType )
  vec4 transformAnimatedVertex(in vec4 v)
  {
    mat4 boneTransform = boneMatrix[in_boneIDs[0]] * in_weights[0];
    boneTransform += boneMatrix[in_boneIDs[1]] * in_weights[1];
    boneTransform += boneMatrix[in_boneIDs[2]] * in_weights[2];
    boneTransform += boneMatrix[in_boneIDs[3]] * in_weights[3];

    return projModelViewMatrix * (boneTransform * v);
  }

void main() 
{            
  vec4 vertex4 = vec4(in_position, 1.0);  
  gl_Position = transformType(vertex4);  
}
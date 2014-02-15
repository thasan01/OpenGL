//sceneShader.vert
#version 330 core

  in vec3 in_vertex;
  //in vec2 in_textCoord;
  //out Data { vec2 textCoord; } DataOut; 

void main() 
{
  //DataOut.textCoord = in_textCoord;
  gl_Position = vec4(in_vertex, 1.0);
}
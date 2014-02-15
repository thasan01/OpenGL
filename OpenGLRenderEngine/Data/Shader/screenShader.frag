#version 330 core
//shader.frag

  uniform sampler2D textureUnit1;

  layout (std140) uniform QuadBlock
  {
    vec2 pixelSize; //{ 1/screenWidth, 1/screenHeight }
  } Quad;

  out vec4 final_Color;

void main() 
{
  
  vec2 pixelPos = gl_FragCoord.xy * Quad.pixelSize;
  final_Color = texture2D( textureUnit1, pixelPos );

}
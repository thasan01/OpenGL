//combineShader.frag
#version 330 core

uniform sampler2D textureUnit1;
uniform sampler2D textureUnit2;

layout (std140) uniform QuadBlock
{
  vec2 pixelSize;
} Quad;


out vec4 finalColor;

void main()
{
  vec2 pixelPos = gl_FragCoord.xy * Quad.pixelSize;
  finalColor = vec4( texture2D(textureUnit1,pixelPos).xyz + texture2D(textureUnit2,pixelPos).xyz, 1.0);
}
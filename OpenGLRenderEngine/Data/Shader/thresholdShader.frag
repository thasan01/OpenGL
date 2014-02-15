//thresholdShader.frag
#version 330 core

uniform sampler2D textureUnit1;
uniform float bloomFactor=1.0;
uniform float intensityThreshold = 0.75;

  layout (std140) uniform QuadBlock
  {
    vec2 pixelSize;
  } Quad;


out vec4 finalColor;

void main()
{

  vec2 pixelPos = gl_FragCoord.xy * Quad.pixelSize;
  float intensity = dot( texture2D(textureUnit1,pixelPos).xyz, vec3(0.33333,0.33333,0.33333) );
  if(intensity > intensityThreshold)  
    //finalColor = vec4(bloomFactor,bloomFactor,bloomFactor,1.0);
    finalColor = texture2D(textureUnit1,pixelPos);
  else 
    finalColor = vec4(0.0,0.0,0.0,1.0);
}
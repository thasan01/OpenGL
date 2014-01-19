#version 330 core
//shader.frag

uniform sampler2D textureUnit1;

uniform vec3 materialDiffuse;
uniform vec3 materialAmbient;
uniform vec3 materialSpecular;
uniform float materialShininess;

uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform vec3 lightPosition;


  in Data
  {
	vec2 textCoord;
	vec3 position;
	vec3 normal;
  } DataIn;

out vec4 colorOut;

void main() 
{
  // Set the output color of our current pixel
  vec4 textel = texture2D( textureUnit1, DataIn.textCoord );

  float dist=length(DataIn.position-lightPosition);   //distance from light-source to surface
  float att=1.0/(1.0+0.1*dist+0.1*dist*dist);    //attenuation (constant,linear,quadric)

  //Ambient
  vec3 ambientColor = lightAmbient * textel.rgb * materialAmbient;
      
  //Diffuse
  vec3 surf2light = normalize(lightPosition - DataIn.position); //surface to light direction
  vec3 norm = normalize(DataIn.normal);
  float diffuseContribution = max(0.0, dot(surf2light, norm));
  vec3 diffuseColor =  ( textel.rgb * materialDiffuse * lightDiffuse) * diffuseContribution;

  //Specular
  //direction from surface position to camera(which is always at the origin)
  vec3 surf2view = normalize(-DataIn.position);
  vec3 reflection = reflect(norm, surf2light);

  float specularContribution = pow(max(0.0,dot(surf2view,reflection)),materialShininess);
  vec3 specularColor = (lightSpecular * materialSpecular) * specularContribution;

  colorOut = vec4((ambientColor + diffuseColor + specularColor), 1.0);
//  colorOut = vec4(1.0, 1.0, 1.0, 1.0);

}
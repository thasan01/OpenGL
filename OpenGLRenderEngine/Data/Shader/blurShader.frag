#version 330 core
//blurShader.frag

uniform sampler2D textureUnit1;
uniform int isVertical;

  layout (std140) uniform QuadBlock
  {
    vec2 pixelSize;
  } Quad;


out vec4 finalColor;


void main()
{
	vec2 pos=gl_FragCoord.xy*Quad.pixelSize;

	float values[9];
	values[0]=0.01;
	values[1]=0.08;
	values[2]=0.01;
	values[3]=0.08;
	values[4]=0.64;
	values[5]=0.08;
	values[6]=0.01;
	values[7]=0.08;
	values[8]=0.01;


	vec4 result;
	if(isVertical==1)
	{
		vec2 curSamplePos=vec2(pos.x,pos.y-4.0*Quad.pixelSize.y);
		for(int i=0;i<9;i++)
		{
			result+=texture2D(textureUnit1,curSamplePos)*values[i];
			curSamplePos.y+=Quad.pixelSize.y;
		}
	}else{
		vec2 curSamplePos=vec2(pos.x-4.0*Quad.pixelSize.x,pos.y);
		for(int i=0;i<9;i++)
		{
			result+=texture2D(textureUnit1,curSamplePos)*values[i];
			curSamplePos.x+=Quad.pixelSize.x;
		}		
	}	





	finalColor = vec4(result.xyz,1.0);
}

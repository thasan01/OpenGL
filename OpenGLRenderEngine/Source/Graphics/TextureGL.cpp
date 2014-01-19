#include "TextureGL.h"
#include <FreeImage\Dist\FreeImage.h>

  unsigned int TextureGL::loadTextureFile(const char* filename)
  {//Original code written by Ben English (benjamin.english@oit.edu)

	unsigned short minFilter=1, magFilter=1;
	unsigned short wrapS=1, wrapT=1, env=0;

	const int MinFilterList[] = { GL_NEAREST, GL_LINEAR,
								 GL_NEAREST_MIPMAP_NEAREST,
								 GL_LINEAR_MIPMAP_NEAREST,
								 GL_NEAREST_MIPMAP_LINEAR,
								 GL_LINEAR_MIPMAP_LINEAR };
                          
	const int MagFilterList[] = {GL_NEAREST, GL_LINEAR};
	const int WrapSTList[]    = {GL_CLAMP, GL_REPEAT};
	const int EnvList[]       = {GL_MODULATE, GL_DECAL, GL_BLEND, GL_REPLACE};

	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	BYTE* bits(0);
	//OpenGL's image ID to map to
	GLuint gl_texID;
	
	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename, 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(filename);
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		return 0;

	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename);
	//if the image failed to load, return failure
	if(!dib)
		return 0;

	//dib = FreeImage_Rotate( dib, 90.0);
	//FreeImage_FlipHorizontal(dib);
	//FreeImage_FlipVertical(dib);

	if ( FreeImage_GetBPP( dib ) != 32 )
	{
		FIBITMAP* oldImage = dib;
		dib = FreeImage_ConvertTo32Bits( oldImage );
		FreeImage_Unload( oldImage );
	}

	//retrieve the image data
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	unsigned int width = FreeImage_GetWidth(dib);
	unsigned int height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if((bits == 0) || (width == 0) || (height == 0))
	{
		FreeImage_Unload(dib);
		return 0;
	}

	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &gl_texID);
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, gl_texID);
	//store the texture data for OpenGL use
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapSTList[ wrapS ] ); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapSTList[ wrapT ]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilterList[ magFilter ]); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilterList[ minFilter ]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, EnvList[ env ]); 

	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);

	//return success
	return gl_texID;
  }

	TextureGL::TextureGL(const string& filename) : m_textureID(TextureGL::loadTextureFile(filename.data())), m_filename(filename)
	{
	}

	TextureGL::~TextureGL()
	{
		GLuint textId = m_textureID;
		glDeleteTextures(1, &textId);
	}

	void TextureGL::bind(unsigned int id) const
	{
		glActiveTexture(GL_TEXTURE0+id);
		glBindTexture(GL_TEXTURE_2D, m_textureID);
	}
	
	void TextureGL::unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}



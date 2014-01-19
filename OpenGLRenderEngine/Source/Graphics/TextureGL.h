#pragma once
#include "../TypeDef.h"
#include <GL\glew.h>
#include <GL\glu.h>
#include <GL\gl.h>

class TextureGL
{
	public:
	TextureGL(const string& filename);
	~TextureGL();

	const string& getFileName() const { return m_filename; }
	unsigned int getID() const { return m_textureID; }

	void bind(unsigned int id=0) const;
	void unbind() const;


	private:
	static unsigned int loadTextureFile(const char* filename);
	unsigned int m_textureID;
	string m_filename;
};
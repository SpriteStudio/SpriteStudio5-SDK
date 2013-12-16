#include <stdio.h>
#include <cstdlib>


#include "stb_image.h"
#include "ssHelper.h"
#include "SSTextureDX9.h"

#if 0
/* =====================================================================================
	テクスチャファイルの読み込み
===================================================================================== */
GLuint	LoadTextureGL( const char* Filename ,int& width , int& height)
{

	int bpp;
	stbi_uc* image = stbi_load( Filename, &width , &height , &bpp , 0 );
	if ( image == 0 ) return 0;

	int target = GL_TEXTURE_RECTANGLE_ARB;
	 
	if (SsUtTextureisPow2(width) &&
		SsUtTextureisPow2(height))
	{
		target = GL_TEXTURE_2D;
	}


	GLuint glyphTexture = 0;
	glGenTextures(1, &glyphTexture);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(target, glyphTexture);

	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	if ( bpp == 4 )
	{
		glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);	
	}else if ( bpp == 3 )
	{
		glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);	
	}
	stbi_image_free (image);

	return glyphTexture;
}
#endif


SSTextureDX9::~SSTextureDX9()
{
	//glDeleteTextures( 1 , &tex );
}

bool SSTextureDX9::Load( const char* filename )
{
	//tex = LoadTextureGL( fname , tex_width , tex_height );
	//return tex != 0;

	return true;
}

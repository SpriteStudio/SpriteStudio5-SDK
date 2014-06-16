#include <stdio.h>
#include <cstdlib>

#ifndef _WIN32
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
    #include <OpenGL/glext.h>
#else
    #include <GL/glew.h>
    #include <GL/GL.h>
#endif

#include "../stb_image.h"
#include "../ssHelper.h"
#include "SSTextureGL.h"


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
	glBindTexture(target, glyphTexture);

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	if ( bpp == 4 )
	{
		stbi_uc *ip = image;
		for ( int i = 0 ; i < width * height ; i++ )
		{
			stbi_uc* r = ip; ip ++;
			stbi_uc* g = ip; ip ++;
			stbi_uc* b = ip; ip ++;
			stbi_uc* a = ip; ip ++;
//			if ( *a == 0 )
			{
				//*r = *g = *b = 0xff;
				int _a = *a;
				int _r = *r;
				int _g = *g;
				int _b = *b;
				*r = ( _r * _a) >> 8 ;
				*g = ( _g * _a) >> 8 ;
				*b = ( _b * _a) >> 8 ;
			}
		}
		glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);	
	}else if ( bpp == 3 )
	{
		glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);	
	}
	stbi_image_free (image);

	return glyphTexture;
}



SSTextureGL::~SSTextureGL()
{
	glDeleteTextures( 1 , &tex );
}

bool SSTextureGL::Load( const char* fname )
{
	//int tex_width;
	//int tex_height;

	tex = LoadTextureGL( fname , tex_width , tex_height );
	return tex != 0;
}

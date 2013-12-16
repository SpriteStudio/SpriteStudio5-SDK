#include <stdio.h>
#include <cstdlib>

#include "stb_image.h"
#include "ssHelper.h"
#include "sshTextureBMP.h"





bool SSTextureBMP::Load( const char* fname )
{
	int bpp;
	stbi_uc* image = stbi_load( fname, &tex_width , &tex_height , &bpp , 0 );
	if ( image == 0 ) return 0;

/*
	if (SsUtTextureisPow2(tex_width) &&
		SsUtTextureisPow2(tex_height))
	{
		target = GL_TEXTURE_2D;
	}
*/
	
	stbi_image_free (image);
	return true;
}

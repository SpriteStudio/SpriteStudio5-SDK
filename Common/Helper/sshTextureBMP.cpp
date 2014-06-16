#include <stdio.h>
#include <cstdlib>

#include "stb_image.h"
#include "ssHelper.h"
#include "sshTextureBMP.h"





bool SSTextureBMP::Load( const char* fname )
{
	int bpp;

	m_filename = "";

	stbi_uc* image = stbi_load( fname, &tex_width , &tex_height , &bpp , 0 );
	if ( image == 0 ) return 0;

	m_filename = fname;
	
	stbi_image_free (image);
	return true;
}

#ifndef __TKTEXTURE__
#define __TKTEXTURE__

#if EMSCRIPTEN
#include <GL/glfw.h>
#else
#include <GLFW/glfw3.h>
#endif
#include "../IsshTexture.h"


class	SSTextureGL : public ISSTexture
{
public:
	GLuint	tex;
	int tex_width;
	int	tex_height;

public:
	SSTextureGL() : tex_width(0) , tex_height(0) , tex(0){}
	virtual ~SSTextureGL();
	bool Load( const char* fname );	

	virtual int	getWidth() { return tex_width; }
	virtual int	getHeight() { return tex_height; }

	virtual ISSTexture* create(){ return new SSTextureGL(); }

};







#endif

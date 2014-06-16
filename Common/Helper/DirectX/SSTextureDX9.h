#ifndef __SSTEXTUREDX9__
#define __SSTEXTUREDX9__


#include "IsshTexture.h"

//テクスチャクラス ( DirectX9 )
class	SSTextureDX9 : public ISSTexture
{
public:
	int tex_width;
	int	tex_height;

public:
	SSTextureDX9() : tex_width(0) , tex_height(0){}
	virtual ~SSTextureDX9();
	bool Load( const char* filename );	

	virtual int	getWidth() { return tex_width; }
	virtual int	getHeight() { return tex_height; }
	
	virtual ISSTexture* create(){ return new SSTextureDX9(); }
};







#endif

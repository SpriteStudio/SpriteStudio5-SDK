#ifndef __TKTEXTURE__
#define __TKTEXTURE__




//テクスチャクラス ( DirectX9 )
class	SSTextureDX9 : public ISSTexture
{
public:
	int tex_width;
	int	tex_height;

public:
	SSTextureDX9() : tex_width(0) , tex_height(0){}
	virtual ~SSTextureDX9();
	bool LoadImage( const char* filename );	
	virtual ISSGraphTexture* create(){ return new SSGraphTextureDX9(); }

	virtual int	getWidth() { return tex_width; }
	virtual int	getHeight() { return tex_height; }
	
};







#endif

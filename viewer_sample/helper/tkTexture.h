#ifndef __TKTEXTURE__
#define __TKTEXTURE__

#include <GLFW/glfw3.h>

#define FOR_OPNEGL 


GLuint	LoadTextureGL( const char* Filename ,int& width , int& height);


//OpenGL/DirectXで共通に使用するためのラッパー
//テクスチャ自体を表すクラス　マネージャは別に後で作る　
//動作検証が目的なので現在はやらない
class	tkTexture
{
public:
#ifdef FOR_OPNEGL
	GLuint	tex;
#endif
	int tex_width;
	int	tex_height;

public:
	tkTexture() : tex_width(0) , tex_height(0) , tex(0){}
	virtual ~tkTexture();
	bool LoadImage( const char* fname );
	
};



#endif

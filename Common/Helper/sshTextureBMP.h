#ifndef __TKTEXTURE__
#define __TKTEXTURE__

#include "IsshTexture.h"



//画像ファイルの縦横サイズを調べる目的等で使用する
//画像ファイルデータは読み捨てる　クラス名が適切でないかも
//画像の加工等を行う場合は拡張をお願いします。
class	SSTextureBMP : public ISSTexture
{
public:
	int tex_width;
	int	tex_height;

	std::string m_filename;

public:
	SSTextureBMP() : tex_width(0) , tex_height(0) , m_filename(""){}
	virtual ~SSTextureBMP(){}
	bool Load( const char* fname );

	virtual int	getWidth() { return tex_width; }
	virtual int	getHeight() { return tex_height; }

	virtual ISSTexture* create(){ return new SSTextureBMP(); }
	virtual const char*	getFilename(){ return m_filename.c_str(); }
};







#endif

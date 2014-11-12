/**
*  SS5PlayerPlatform.h
*/

#ifndef SS5PlayerPlatform_h
#define SS5PlayerPlatform_h
#include "SS5Player.h"
#include <stdio.h>
#include <string>
#include <windows.h>

#define CELL_MAX (100)
struct CELLDATA {
	int index;
	char name[MAX_PATH];
};

struct PARTDATA {
	int texid;	//テクスチャID
	int visble;	//非表示フラグ
	int x;		//表示位置X
	int y;		//表示位置Y
	int sx;		//テクスチャ内の開始位置X
	int sy;		//テクスチャ内の開始位置Y
	int h;		//テクスチャの幅
	int w;		//テクスチャの高さ
	int zoomx;	//拡大率X
	int zoomy;	//拡大率Y
	int rot;	//回転角度
	int alpha;	//透明度
	int brend;	//ブレンドモード
	int flipx;	//横反転
	int flipy;	//縦反転
	int sizex;	//Xサイズ
	int sizey;	//Yサイズ
	int boundingRadius;	//当たり半径
};

namespace ss
{
	struct State;
	struct UserData;
	class Player;

	extern unsigned char* SSFileOpen(const char* pszFileName, const char* pszMode, unsigned long * pSize);
	extern long SSTextureLoad(const char* pszFileName);
	extern bool SSTextureRelese(long handle);
	extern bool isAbsolutePath(const std::string& strPath);
	extern void SSDrawSprite(State state);
	extern bool SSGetTextureSize(long handle, int &w, int &h);
	extern void SSonUserData(Player *player, UserData *userData);
	extern void SSPlayEnd(Player *player);

};	// namespace ss

#endif

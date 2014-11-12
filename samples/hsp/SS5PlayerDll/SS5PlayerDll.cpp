#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "ssbpLib/SS5Player.h"
#include "ssbpLib/SS5PlayerPlatform.h"


/**
* SS5Player for HSP
*/
//#define OUTPUT_DEBUG_LOG	//有効にするとデバッグ出力
void logout(const char *str);

//-------------------------------------------------------------------------------
//	DLLエントリー
//-------------------------------------------------------------------------------
BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}



/// SS5プレイヤー
#define SSPLAYER_MAX (32)
ss::Player *ssplayer[SSPLAYER_MAX];

int buffcount = 0;
int texindex = 0;


CELLDATA celldata[CELL_MAX];
PARTDATA partdata[128];
float fps_dt = 0.0166f;

//SS5P for HSPインターフェース
//初期化
INT WINAPI SSPlayer_initlize(void)
{
	logout("SSPlayer_initlize---------------------------------\n");

	memset(celldata, 0, sizeof(CELLDATA));
	int idx;
	for (idx = 0; idx < CELL_MAX; idx++)
	{
		celldata[idx].index = -1;
	}


	buffcount = 0;
	texindex = 0;

	for (idx = 0; idx < SSPLAYER_MAX; idx++)
	{
		ssplayer[idx] = NULL;
	}
	logout("SSPlayer_initlize_end\n");
	return 0;
}

//FPS設定
INT WINAPI SSPlayer_setFps(int fps)
{
	logout("SSPlayer_setFps\n");

	fps_dt = 1.0f / (float)fps;
	logout("SSPlayer_setFps\n");
	return 0;
}

//DLLで管理しているプレイヤー数を取得
INT WINAPI SSPlayer_getPlayerNum(void)
{
	return SSPLAYER_MAX;
}


//sspb読み込み
INT WINAPI SSPlayer_addData(char *name)
{
	logout("SSPlayer_addData\n");

	ss::ResourceManager *resman;
	resman = ss::ResourceManager::getInstance();
	//それぞれのプラットフォームに合わせたパスへ変更してください。
	std::string ret = resman->addData(name);
	logout("SSPlayer_addData_end\n");
	return 0;
}

//テクスチャ番号を取得
INT WINAPI SSPlayer_get_texureIndex(int idx )
{
	return celldata[idx].index;
}
//テクスチャ名を取得
INT WINAPI SSPlayer_get_texureName(int idx, char *name)
{
	sprintf_s(name, MAX_PATH, "%s", celldata[idx].name);
	return 0;
}

//プレイヤー作成
INT WINAPI SSPlayer_play(int idx, char *data_name, char *motion_name)
{
	logout("SSPlayer_create\n");
	
	if ( ssplayer[idx] == NULL )
	{
		ssplayer[idx] = ss::Player::create();	//プレイヤー作成
	}
	ssplayer[idx]->setData(data_name);		        // ssbpファイル名（拡張子不要）

	logout("SSPlayer_create_1\n");
	char str[128];
	sprintf_s(str, 128, "%s\n", motion_name );
	logout(str);


	//再生するモーションを設定
	ssplayer[idx]->play(motion_name);				 // アニメーション名を指定(ssae名/

	logout("SSPlayer_create_end\n");
	return 0;

}

//プレイヤー更新
INT WINAPI SSPlayer_update(void)
{
//	logout("SSPlayer_update\n");

	int idx;
	for (idx = 0; idx < SSPLAYER_MAX; idx++)
	{
		if ( ssplayer[idx] != NULL )
		{
			ssplayer[idx]->update(fps_dt);		//アニメの更新
		}
	}
	return 0;
}

//パーツ数の取得
INT WINAPI SSPlayer_get_partNum(int idx)
{
	int rc = 0;
	if (ssplayer[idx] != NULL)
	{
		rc = ssplayer[idx]->getPartNum();

	}
	return rc;
}

//パーツステータスの取得
INT WINAPI SSPlayer_get_partStatus(int idx, int partidx, PARTDATA *pt)
{

	pt->texid = -1;	//テクスチャID
	if (ssplayer[idx] != NULL)
	{

		ss::State state;
		state = ssplayer[idx]->getPartState(partidx);
		pt->texid = state.texture;					//00:テクスチャID
		pt->visble = 0;
		if ( state.isVisibled == true )				//01:非表示フラグ
		{
			pt->visble = 1;
		}
		pt->x = (int)state.mat[12];					//02:表示位置X
		pt->y = (int)state.mat[13];					//03:表示位置Y
		pt->sx = (int)state.rect.origin.x;			//04:テクスチャ内の開始位置X
		pt->sy = (int)state.rect.origin.y;			//05:テクスチャ内の開始位置Y
		pt->h = (int)state.rect.size.width;			//06:画像の幅
		pt->w = (int)state.rect.size.height;		//07:画像の高さ
		pt->zoomx = (int)( state.scaleX * 100.0f );	//08:拡大率X
		pt->zoomy = (int)( state.scaleY * 100.0f );	//09:拡大率Y
		pt->rot =   (int)( state.rotationZ );		//10:回転角度
		pt->alpha = state.opacity;					//11:透明度
		pt->brend = state.blendfunc;				//12:ブレンドモード
		pt->flipx = 0;								//13:横反転
		if (state.flipX == true )
		{
			pt->flipx = 1;							//13:横反転
		}
		pt->flipy = 0;								//14:縦反転
		if (state.flipY == true)
		{
			pt->flipy = 1;							//14:縦反転
		}
		pt->sizex = (int)(state.size_X);			//15:Xサイズ
		pt->sizey = (int)(state.size_Y);			//16:Yサイズ
		pt->boundingRadius = (int)(state.boundingRadius);	//17:当たり半径

	}
	//	logout("SSPlayer_update\n");

	return 0;
}

//プレイヤー表示位置設定
INT WINAPI SSPlayer_setPosition(int idx, int x, int y)
{
	if (ssplayer[idx] != NULL)
	{
		ssplayer[idx]->setPosition((float)x, (float)y);
	}
	return 0;
}

//プレイヤー表示角度設定
INT WINAPI SSPlayer_setRotation(int idx, double rotx, double roty, double rotz)
{
	if (ssplayer[idx] != NULL)
	{
		ssplayer[idx]->setRotation((float)rotx, (float)roty, (float)rotz );
	}
	return 0;
}

//プレイヤー表示スケール設定
INT WINAPI SSPlayer_setScale(int idx, double zoomx, double zoomy)
{
	if (ssplayer[idx] != NULL)
	{
		ssplayer[idx]->setScale((float)zoomx, (float)zoomy);
	}
	return 0;
}

//プレイヤー透明度設定
INT WINAPI SSPlayer_setAlpha(int idx, int a)
{
	if (ssplayer[idx] != NULL)
	{
		ssplayer[idx]->setAlpha(a);
	}
	return 0;
}

//プレイヤー反転設定
INT WINAPI SSPlayer_setFlip(int idx, int flipX, int flipY)
{
	bool fx =  false;
	if (flipX != 0 )
	{
		fx = true;
	}
	bool fy = false;
	if (flipY != 0)
	{
		fy = true;
	}
	if (ssplayer[idx] != NULL)
	{
		ssplayer[idx]->setFlip(fx, fy);
	}
	return 0;
}

//プレイヤーポーズ
INT WINAPI SSPlayer_pause(int idx)
{
	if (ssplayer[idx] != NULL)
	{
		ssplayer[idx]->pause();
	}
	return 0;
}
//プレイヤー再開
INT WINAPI SSPlayer_resume(int idx)
{
	if (ssplayer[idx] != NULL)
	{
		ssplayer[idx]->resume();
	}
	return 0;
}
//プレイヤーフレーム指定
INT WINAPI SSPlayer_setFrameNo(int idx, int FrameNo)
{
	if (ssplayer[idx] != NULL)
	{
		ssplayer[idx]->setFrameNo(FrameNo);
	}
	return 0;
}
//プレイヤーアニメ総フレーム取得
INT WINAPI SSPlayer_getMaxFrame(int idx)
{
	if (ssplayer[idx] != NULL)
	{
		return ( ssplayer[idx]->getMaxFrame() );
	}
	return 0;
}
//プレイヤーアニメ再生フレーム取得
INT WINAPI SSPlayer_getFrameNo(int idx)
{
	if (ssplayer[idx] != NULL)
	{
		return (ssplayer[idx]->getFrameNo() );
	}
	return 0;
}
//名前からパーツステータスを取得
INT WINAPI SSPlayer_get_namePartStatus(int idx, char* name, int frame, PARTDATA *pt)
{
	pt->texid = -1;	//テクスチャID
	if (ssplayer[idx] != NULL)
	{

		ss::State state;
		state = ssplayer[idx]->getPartState(name, frame);
		pt->texid = state.texture;					//00:テクスチャID
		pt->visble = 0;
		if (state.isVisibled == true)				//01:非表示フラグ
		{
			pt->visble = 1;
		}
		pt->x = (int)state.mat[12];					//02:表示位置X
		pt->y = (int)state.mat[13];					//03:表示位置Y
		pt->sx = (int)state.rect.origin.x;			//04:テクスチャないの開始位置X
		pt->sy = (int)state.rect.origin.y;			//05:テクスチャないの開始位置Y
		pt->h = (int)state.rect.size.width;			//06:テクスチャないの開始終了X
		pt->w = (int)state.rect.size.height;		//07:テクスチャないの開始終了Y
		pt->zoomx = (int)(state.scaleX * 100.0f);	//08:拡大率X
		pt->zoomy = (int)(state.scaleY * 100.0f);	//09:拡大率Y
		pt->rot = (int)(state.rotationZ);		//10:回転角度
		pt->alpha = state.opacity;					//11:透明度
		pt->brend = state.blendfunc;				//12:ブレンドモード
		pt->flipx = 0;								//13:横反転
		if (state.flipX == true)
		{
			pt->flipx = 1;							//13:横反転
		}
		pt->flipy = 0;								//14:縦反転
		if (state.flipY == true)
		{
			pt->flipy = 1;							//14:縦反転
		}
		pt->sizex = (int)(state.size_X);			//15:Xサイズ
		pt->sizey = (int)(state.size_Y);			//16:Yサイズ
		pt->boundingRadius = (int)(state.boundingRadius);	//17:当たり半径

	}
	//	logout("SSPlayer_update\n");

	return 0;
}

//終了処理
INT WINAPI SSPlayer_Delete(void)
{
	int idx;
	for (idx = 0; idx < SSPLAYER_MAX; idx++)
	{
		if ( ssplayer[idx] )
		{
			delete(ssplayer[idx]);
			ssplayer[idx] = NULL;
		}
	}
	ss::ResourceManager *resman;
	resman = ss::ResourceManager::getInstance();
	delete (resman);
	return 0;
}


//ログ出力
void logout(const char *str)
{
#ifdef OUTPUT_DEBUG_LOG
	FILE *fp;	

	if ((fp = fopen("sshsplog.txt", "a")) == NULL) {
		printf("file open error!!\n");
		exit(EXIT_FAILURE);	
	}

	fputs(str, fp);
	fclose(fp);	
#endif
}
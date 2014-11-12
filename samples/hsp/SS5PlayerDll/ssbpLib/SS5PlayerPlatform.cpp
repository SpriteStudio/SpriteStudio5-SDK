// 
//  SS5Platform.cpp
//
#include "SS5PlayerPlatform.h"

/**
* 各プラットフォームに合わせて処理を作成してください
* DXライブラリ用に作成されています。
*/
//#include "DxLib.h"
extern int buffcount;
extern int texindex;
extern CELLDATA celldata[CELL_MAX];

namespace ss
{

	/**
	* ファイル読み込み
	*/
	unsigned char* SSFileOpen(const char* pszFileName, const char* pszMode, unsigned long * pSize)
	{
		unsigned char * pBuffer = NULL;
		SS_ASSERT2(pszFileName != NULL && pSize != NULL && pszMode != NULL, "Invalid parameters.");
		*pSize = 0;
		do
		{
		    // read the file from hardware
			FILE *fp = fopen(pszFileName, pszMode);
		    SS_BREAK_IF(!fp);
		    
		    fseek(fp,0,SEEK_END);
		    *pSize = ftell(fp);
		    fseek(fp,0,SEEK_SET);
		    pBuffer = new unsigned char[*pSize];
		    *pSize = fread(pBuffer,sizeof(unsigned char), *pSize,fp);
		    fclose(fp);
		} while (0);
		if (! pBuffer)
		{

			std::string msg = "Get data from file(";
		    msg.append(pszFileName).append(") failed!");
		    
		    SSLOG("%s", msg.c_str());

		}
		return pBuffer;
	}

	/**
	* テクスチャの読み込み
	*/
	long SSTextureLoad(const char* pszFileName )
	{
		/**
		* テクスチャ管理用のユニークな値を返してください。
		* テクスチャの管理はゲーム側で行う形になります。
		* テクスチャにアクセスするハンドルや、テクスチャを割り当てたバッファ番号等になります。
		*
		* プレイヤーはここで返した値とパーツのステータスを引数に描画を行います。
		*/
		long rc = texindex;
		celldata[buffcount].index = texindex;
		sprintf_s(celldata[buffcount].name, MAX_PATH, "%s", pszFileName);

		buffcount++;
		texindex++;

//		rc = (long)LoadGraph(pszFileName);

		return rc;
	}
	
	/**
	* テクスチャの解放
	*/
	bool SSTextureRelese(long handle)
	{
		/// 解放後も同じ番号で何度も解放処理が呼ばれるので、例外が出ないように作成してください。
		bool rc = true;
/*
		if ( DeleteGraph((int)handle) == -1 )
		{
			rc = false;
		}
*/
		return rc ;
	}

	/**
	* テクスチャのサイズを取得
	* テクスチャのUVを設定するのに使用します。
	*/
	bool SSGetTextureSize(long handle, int &w, int &h)
	{
/*
		if (GetGraphSize(handle, &w, &h) == -1)
		{
			return false;
		}
*/
		return true;
	}

	/**
	* スプライトの表示
	*/
	void SSDrawSprite(State state)
	{
#if 0
		//未対応機能
		//ステータスから情報を取得し、各プラットフォームに合わせて機能を実装してください。
		//X回転、Y回転、上下反転、カラーブレンド（一部のみ）
		//頂点変形、Xサイズ、Yサイズ
		float x = state.mat[12];	/// 表示座標はマトリクスから取得します。
		float y = state.mat[13];	/// 表示座標はマトリクスから取得します。
		float rotationZ = RadianToDegree(state.rotationZ);		/// 回転値
		float scaleX = state.scaleX;							/// 拡大率
		float scaleY = state.scaleY;							/// 拡大率

		//描画ファンクション
		//
		switch (state.blendfunc)
		{
			case BLEND_MIX:		///< 0 ブレンド（ミックス）
				if (state.opacity == 255)
				{
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, state.opacity);
				}
				else
				{
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, state.opacity);
				}
				break;
			case BLEND_MUL:		///< 1 乗算
				SetDrawBlendMode(DX_BLENDMODE_MULA, state.opacity);
				break;
			case BLEND_ADD:		///< 2 加算
				SetDrawBlendMode(DX_BLENDMODE_ADD, state.opacity);
				break;
			case BLEND_SUB:		///< 3 減算
				SetDrawBlendMode(DX_BLENDMODE_SUB, state.opacity);
				break;

		}

		if (state.flags & PART_FLAG_COLOR_BLEND)
		{
			//RGBのカラーブレンドを設定
			//厳密に再現するには専用のシェーダーを使い、テクスチャにカラー値を合成する必要がある
			//作成する場合はssShader_frag.h、CustomSpriteのコメントとなってるシェーダー処理を参考にしてください。
			if (state.colorBlendType == VERTEX_FLAG_ONE)
			{
				//単色カラーブレンド
			}
			else
			{
				//頂点カラーブレンド
				//未対応
			}
			switch (state.colorBlendFunc)
			{
			case BLEND_MIX:
				break;
			case BLEND_MUL:		///< 1 乗算
				// ブレンド方法は乗算以外未対応
				// とりあえず左上の色を反映させる
				SetDrawBright(state.quad.tl.colors.r, state.quad.tl.colors.g, state.quad.tl.colors.b);
				break;
			case BLEND_ADD:		///< 2 加算
				break;
			case BLEND_SUB:		///< 3 減算
				break;
			}

		}

		/**
		* DXライブラリはXとY同時拡大なので、とりあえずXスケールを使用する
		* DXライブラリはY反転できないので未対応
		* DrawRectRotaGraphはxとyが中心になるように、テクスチャの矩形を表示します。
		*/
		DrawRectRotaGraph(
			(int)x, (int)y,	//この座標が画像の中心になります。
			(int)state.rect.origin.x, (int)state.rect.origin.y, (int)state.rect.size.width, (int)state.rect.size.height,
			scaleX, rotationZ,
			state.texture, TRUE, state.flipX
			);

		SetDrawBright(255, 255, 255);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);	//ブレンドステートを通常へ戻す
#endif
	}

	/**
	* ユーザーデータの取得
	*/
	void SSonUserData(Player *player, UserData *userData)
	{
		//ゲーム側へユーザーデータを設定する関数を呼び出してください。
	}

	/**
	* ユーザーデータの取得
	*/
	void SSPlayEnd(Player *player)
	{
		//ゲーム側へアニメ再生終了を設定する関数を呼び出してください。
	}


	/**
	* 文字コード変換
	*/ 
/*
	std::string utf8Togbk(const char *src)
	{

		int len = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
		unsigned short * wszGBK = new unsigned short[len + 1];
		memset(wszGBK, 0, len * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)src, -1, (LPWSTR)wszGBK, len);

		len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
		char *szGBK = new char[len + 1];
		memset(szGBK, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
		std::string strTemp(szGBK);
		if (strTemp.find('?') != std::string::npos)
		{
			strTemp.assign(src);
		}
		delete[]szGBK;
		delete[]wszGBK;
		return strTemp;
	}
*/
	/**
	* windows用パスチェック
	*/ 
	bool isAbsolutePath(const std::string& strPath)
	{
/*
		std::string strPathAscii = utf8Togbk(strPath.c_str());
		if (strPathAscii.length() > 2
			&& ((strPathAscii[0] >= 'a' && strPathAscii[0] <= 'z') || (strPathAscii[0] >= 'A' && strPathAscii[0] <= 'Z'))
			&& strPathAscii[1] == ':')
		{
			return true;
		}
*/
		return false;
	}

};

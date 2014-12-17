// 
//  SS5Platform.cpp
//
#include "SS5PlayerPlatform.h"

/**
* 各プラットフォームに合わせて処理を作成してください
* DXライブラリ用に作成されています。
*/
//#include "DxLib.h"
TexureData texdata[TEXDATA_MAX];
PARTDATA partdata[PARTDATA_MAX];
int partdata_index;

namespace ss
{
	/**
	* ファイル読み込み
	*/
	unsigned char* SSFileOpen(const char* pszFileName, const char* pszMode, unsigned long * pSize)
	{
		//テクスチャデータの初期化をここでやってしまう。
		int i;
		for (i = 0; i < TEXDATA_MAX; i++)
		{
			texdata[i].id = -1;
		}

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
	int idx = 0;
	long SSTextureLoad(const char* pszFileName )
	{
		/**
		* テクスチャ管理用のユニークな値を返してください。
		* テクスチャの管理はゲーム側で行う形になります。
		* テクスチャにアクセスするハンドルや、テクスチャを割り当てたバッファ番号等になります。
		*
		* プレイヤーはここで返した値とパーツのステータスを引数に描画を行います。
		*/
		long rc = idx;
		
		texdata[idx].id = idx;
		texdata[idx].name = pszFileName;


		idx++;

		return rc;
	}
	
	/**
	* テクスチャの解放
	*/
	bool SSTextureRelese(long handle)
	{
		/// 解放後も同じ番号で何度も解放処理が呼ばれるので、例外が出ないように作成してください。
		bool rc = true;
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
	void SSDrawSprite(State state, char* name)
	{
		//出力バッファへコピー
		partdata[partdata_index].name = name;						/// このフレームで更新が行われるステータスのフラグ
		partdata[partdata_index].flags = state.flags;						/// このフレームで更新が行われるステータスのフラグ
		partdata[partdata_index].cellIndex = state.cellIndex;					/// パーツに割り当てられたセルの番号
		partdata[partdata_index].x = state.mat[12];						/// SS5アトリビュート：X座標
		partdata[partdata_index].y = state.mat[13];						/// SS5アトリビュート：X座標
		partdata[partdata_index].anchorX = state.anchorX;					/// 原点Xオフセット＋セルに設定された原点オフセットX
		partdata[partdata_index].anchorY = state.anchorY;					/// 原点Yオフセット＋セルに設定された原点オフセットY
		partdata[partdata_index].rotationX = state.rotationX;				/// X回転（親子関係計算済）
		partdata[partdata_index].rotationY = state.rotationY;				/// Y回転（親子関係計算済）
		partdata[partdata_index].rotationZ = state.rotationZ;				/// Z回転（親子関係計算済）
		partdata[partdata_index].scaleX = state.scaleX;					/// Xスケール（親子関係計算済）
		partdata[partdata_index].scaleY = state.scaleY;					/// Yスケール（親子関係計算済）
		partdata[partdata_index].opacity = state.opacity;					/// 不透明度（0～255）（親子関係計算済）
		partdata[partdata_index].size_X = state.size_X;					/// SS5アトリビュート：Xサイズ
		partdata[partdata_index].size_Y = state.size_Y;					/// SS5アトリビュート：Xサイズ
		partdata[partdata_index].uv_move_X = state.uv_move_X;				/// SS5アトリビュート：UV X移動
		partdata[partdata_index].uv_move_Y = state.uv_move_Y;				/// SS5アトリビュート：UV Y移動
		partdata[partdata_index].uv_rotation = state.uv_rotation;				/// SS5アトリビュート：UV 回転
		partdata[partdata_index].uv_scale_X = state.uv_scale_X;				/// SS5アトリビュート：UV Xスケール
		partdata[partdata_index].uv_scale_Y = state.uv_scale_Y;				/// SS5アトリビュート：UV Yスケール
		partdata[partdata_index].boundingRadius = state.boundingRadius;			/// SS5アトリビュート：当たり半径
		partdata[partdata_index].colorBlendFunc = state.colorBlendFunc;				/// SS5アトリビュート：カラーブレンドのブレンド方法
		partdata[partdata_index].colorBlendType = state.colorBlendType;				/// SS5アトリビュート：カラーブレンドの単色か頂点カラーか。
		partdata[partdata_index].flipX = state.flipX;						/// 横反転（親子関係計算済）
		partdata[partdata_index].flipY = state.flipY;						/// 縦反転（親子関係計算済）
		partdata[partdata_index].isVisibled = state.isVisibled;				/// 非表示（親子関係計算済）
		partdata[partdata_index].instancerotationX = state.instancerotationX;		/// インスタンスパーツに設定されたX回転
		partdata[partdata_index].instancerotationY = state.instancerotationY;		/// インスタンスパーツに設定されたY回転
		partdata[partdata_index].instancerotationZ = state.instancerotationZ;		/// インスタンスパーツに設定されたZ回転
		partdata[partdata_index].texture = state.texture;					/// セルに対応したテクスチャ番号（ゲーム側で管理している番号を設定する）
		partdata[partdata_index].rect.x = (int)state.rect.origin.x;						/// セルに対応したテクスチャ内の表示領域（開始座標、幅高さ）
		partdata[partdata_index].rect.y = (int)state.rect.origin.y;						/// セルに対応したテクスチャ内の表示領域（開始座標、幅高さ）
		partdata[partdata_index].rect.w = (int)state.rect.size.width;						/// セルに対応したテクスチャ内の表示領域（開始座標、幅高さ）
		partdata[partdata_index].rect.h = (int)state.rect.size.height;						/// セルに対応したテクスチャ内の表示領域（開始座標、幅高さ）
		partdata[partdata_index].blendfunc = state.blendfunc;					/// パーツに設定されたブレンド方法
		partdata[partdata_index].quad = state.quad;								/// 頂点情報
		memcpy( partdata[partdata_index].mat, state.mat, sizeof( state.mat ) );					/// パーツの位置を算出するためのマトリクス（親子関係計算済）

		partdata_index++;


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
	* windows用パスチェック
	*/ 
	bool isAbsolutePath(const std::string& strPath)
	{
		return true;
	}

};

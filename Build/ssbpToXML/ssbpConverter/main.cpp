#include <stdio.h>
#include <tchar.h>
#include <string>
#include <locale.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "SSPlayer\SS5Player.h"

void init(void);
void outputdata(void);
void relese(void);
void outputVal(FILE *fp, int val);
void outputVal(FILE *fp, float val);
std::string toShortString(float value);

std::string open_filename;
std::string output_filename;
std::string ssbp_name;
std::string ssae_name;


/// SS5プレイヤー
ss::Player *ssplayer;
ss::ResourceManager *resman;

bool err;

extern TexureData texdata[TEXDATA_MAX];
extern int partdata_index;
extern PARTDATA partdata[PARTDATA_MAX];


/**
* メイン関数
*/
int main(int argc, char *argv[])
{
	//引数チェック
	int i;
	bool ishelp = false;
	int inputcount = 0;
	err = false;

	if (argc == 1)
	{
		ishelp = true;
	}
	for (i = 1; i < argc; i++)
	{
		if (
			(strcmp(argv[i], "-h") == 0)
		 || (strcmp(argv[i], "-H") == 0)
			)
		{
			//ヘルプ表示
			ishelp = true;
		}
		else
		{
			switch (inputcount)
			{
			case 0:
				open_filename = argv[i];
				inputcount++;
				break;
//			case 1:
//				ssae_name = argv[i];
//				inputcount++;
//				break;
			}
		}
	}
	//拡張子チェック
	if (open_filename.find(".ssbp") == std::string::npos)
	{
		err = true;	//エラーが出たら処理中断
	}

	if (inputcount == 1)
	{
		//出力ファイル名を作成
		output_filename = open_filename.substr(0, open_filename.length() - 5);
		output_filename = output_filename + ".xml";
	}
	else
	{
		err = true;	//エラーが出たら処理中断
	}



	if (ishelp == true)
	{
		printf("第一引数：ssbpのパス\r\n");
		printf("オプション\r\n");
		printf("-h：ヘルプ表示\r\n");
	}
	else
	{
		if (err == false)
		{
			/// プレイヤー初期化
			init();
			//ファイル出力
			outputdata();
			/// プレイヤー終了処理
			relese();
		}
	}

	return 0;				// ソフトの終了 
}

void init( void )
{
	/**********************************************************************************

	SSアニメ表示のサンプルコード
	Visual Studio Express 2013 for Windows Desktop、DXライブラリで動作を確認しています。
	ssbpとpngがあれば再生する事ができますが、Resourcesフォルダにsspjも含まれています。

	**********************************************************************************/
	//リソースマネージャの作成
	resman = ss::ResourceManager::getInstance();
	//プレイヤーの作成
	ssplayer = ss::Player::create();

	//アニメデータをリソースに追加
	resman->addData(open_filename);

	int fpos = open_filename.rfind("\\");
	if ( fpos == std::string::npos)
	{
		//￥がない場合はそのまま
		ssbp_name = open_filename;
	}
	else
	{
		ssbp_name = open_filename.substr(fpos + 1);
		ssbp_name = ssbp_name.substr(0, ssbp_name.length() - 5);	//拡張子とる
	}

	//プレイヤーにリソースを割り当て
	ssplayer->setData(ssbp_name);        // ssbpファイル名（拡張子不要）
	//表示位置を設定
	ssplayer->setPosition(0, 0);
	//スケール設定
	ssplayer->setScale(1.0f, 1.0f);
	//回転を設定
	ssplayer->setRotation(0.0f, 0.0f, 0.0f);
	//透明度を設定
	ssplayer->setAlpha(255);
	//反転を設定
	ssplayer->setFlip(false, false);

}


/**
* プレイヤー終了処理
*/
void relese( void )
{
	//SS5Playerの削除
	delete (ssplayer);	
	delete (resman);
}



//セルデータの出力
void outputdata(void)
{
	FILE *fp;
	wchar_t buf[1024];
	// ファイルのオープン
	fopen_s(&fp, output_filename.c_str(), "w,ccs=UTF-8");
	if (fp == NULL)
	{
		//エラー
		printf("file open error!!\n");
		err = true;
	}
	if (err == false)
	{
		//ロケール指定
		setlocale(LC_ALL, "japanese");

		//ヘッダ出力
		fputws(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"), fp);
		fputws(_T("<ssbx version=\"1.0\">\n"), fp);


		{
			//アニメーション情報
			wchar_t	name[256];
			size_t wLen = 0;
			mbstowcs_s(&wLen, name, 256, ssbp_name.c_str(), _TRUNCATE);
			swprintf_s(buf, 1024, _T("  <name>%s</name>\n"), name);
			fputws(buf, fp);

			{
				//セル情報
				fputws(_T("  <textures>\n"), fp);
				int i = 0;
				for (i = 0; i < TEXDATA_MAX; i++)
				{
					if (texdata[i].id == -1)
					{
						break;	//データ終わり
					}
					wchar_t	name[256];
					size_t wLen = 0;
					mbstowcs_s(&wLen, name, 256, texdata[i].name.c_str(), _TRUNCATE);

					swprintf_s(buf, 1024, _T("      <texture id=\"%d\" name=\"%s\" />\n"), texdata[i].id, name);
					fputws(buf, fp);
				}
				fputws(_T("  </textures>\n"), fp);
			}
		}

		ss::ResourceSet* rs = resman->getData(ssbp_name);

		for (std::map<std::string, ss::AnimeRef*>::iterator itpairstri = rs->animeCache->_dic.begin(); itpairstri != rs->animeCache->_dic.end(); itpairstri++)
		{
			// イテレータは pair<const string, int> 型なので、
			std::string strKey = itpairstri->first;     // イテレータからキーが得られる。


			if (strKey.find("/") == std::string::npos)	//ssae名が含まれていない場合はスキップ
			{
				continue;
			}


			//再生するモーションを設定
			ssplayer->play(strKey);				 // アニメーション名を指定(ssae名/アニメーション名も可能、詳しくは後述)
			ssplayer->pause();

			fputws(_T("  <motion>\n"), fp);
			{
				//モーション情報
				wchar_t	name[256];
				size_t wLen = 0;
				mbstowcs_s(&wLen, name, 256, strKey.c_str(), _TRUNCATE);
				swprintf_s(buf, 1024, _T("    <name>%s</name>\n"), name);
				fputws(buf, fp);

				swprintf_s(buf, 1024, _T("    <fps>%d</fps>\n"), ssplayer->getFPS());
				fputws(buf, fp);
				swprintf_s(buf, 1024, _T("    <maxframe>%d</maxframe>\n"), ssplayer->getMaxFrame());
				fputws(buf, fp);

			}

			{
				//フレームデータ
				fputws(_T("    <framedata>\n"), fp);
				int maxframe = ssplayer->getMaxFrame();
				int i; 
				for (i = 0; i < maxframe; i++)
				{
					swprintf_s(buf, 1024, _T("      <frame time=\"%d\">\n"), i);
					fputws(buf, fp);

					ssplayer->setFrameNo(i);
					ssplayer->update(0);


					partdata_index = 0;	//パーツインデックスの初期化
					int idx = 0;
					for (idx = 0; idx < PARTDATA_MAX; idx++ )
					{
						partdata[idx].texture = -2;	//データなし
					}


					ssplayer->draw();	//パーツステータスの取得

					//データ書き出し
					idx = 0;
					while (1)
					{
						if ( partdata[idx].texture == -2 )
						{
							//データ終了
							break;
						}
						//パーツステートを書き出す
						fputws(_T("        <partdata"), fp);
						
						{
							wchar_t	name[256];
							size_t wLen = 0;
							mbstowcs_s(&wLen, name, 256, partdata[idx].name.c_str(), _TRUNCATE);
							swprintf_s(buf, 1024, _T(" NAME=\"%s\""), name);
							fputws(buf, fp);
						}

	//					if (partdata[idx].x != 0.0f )
						{
							fputws(_T(" X="), fp);
							outputVal(fp, partdata[idx].x);
						}

	//					if (partdata[idx].y != 0.0f)
						{
							fputws(_T(" Y="), fp);
							outputVal(fp, partdata[idx].y);
						}

	//					if (partdata[idx].rotationZ != 0.0f)
						{
							fputws(_T(" ROTZ="), fp);
							outputVal(fp, partdata[idx].rotationZ);
						}

	//					if (partdata[idx].scaleX != 0.0f)
						{
							fputws(_T(" SCLX="), fp);
							outputVal(fp, partdata[idx].scaleX);
						}

	//					if (partdata[idx].scaleY != 0.0f)
						{
							fputws(_T(" SCLY="), fp);
							outputVal(fp, partdata[idx].scaleY);
						}

	//					if (partdata[idx].opacity != 0)
						{
							fputws(_T(" ALPH="), fp);
							outputVal(fp, partdata[idx].opacity);
						}

	//					if (partdata[idx].size_X != 0.0f)
						{
							fputws(_T(" SIZX="), fp);
							outputVal(fp, partdata[idx].size_X);
						}

	//					if (partdata[idx].size_Y != 0.0f)
						{
							fputws(_T(" SIZY="), fp);
							outputVal(fp, partdata[idx].size_Y);
						}

	//					if (partdata[idx].boundingRadius != 0.0f)
						{
							fputws(_T(" BORA="), fp);
							outputVal(fp, partdata[idx].boundingRadius);
						}
	/*
						swprintf_s(buf, 1024, _T(" colorBlendFunc=\"%d\""), partdata[idx].colorBlendFunc);
						fputws(buf, fp);

						swprintf_s(buf, 1024, _T(" colorBlendType=\"%d\""), partdata[idx].colorBlendType);
						fputws(buf, fp);
	*/
	//					if (partdata[idx].flipX != 0)
						{
							fputws(_T(" FLPX="), fp);
							outputVal(fp, partdata[idx].flipX);
						}

	//					if (partdata[idx].flipY != 0)
						{
							fputws(_T(" FLPY="), fp);
							outputVal(fp, partdata[idx].flipY);
						}

						fputws(_T(" TXID="), fp);
						outputVal(fp, partdata[idx].texture);

						fputws(_T(" RECX="), fp);
						outputVal(fp, partdata[idx].rect.x);
						fputws(_T(" RECY="), fp);
						outputVal(fp, partdata[idx].rect.y);
						fputws(_T(" RECW="), fp);
						outputVal(fp, partdata[idx].rect.w);
						fputws(_T(" RECH="), fp);
						outputVal(fp, partdata[idx].rect.h);

	//					if (partdata[idx].blendfunc != 0)
						{
							fputws(_T(" BLED="), fp);
							outputVal(fp, partdata[idx].blendfunc);
						}


						fputws(_T(" />\n"), fp);

						idx++;
					}
					fputws(_T("      </frame>\n"), fp);
				}
				fputws(_T("    </framedata>\n"), fp);
			}
			fputws(_T("  </motion>\n"), fp);
		}
		//フッダ出力
		fputws(_T("</ssbx>\n"), fp);
		fputws(_T("\n"), fp);

	}
	fclose(fp);	//ファイルのクローズ
}

void outputVal(FILE *fp, int val)
{
	wchar_t buf[1024];

	swprintf_s(buf, 1024, _T("\"%d\""), val);
	fputws(buf, fp);
}

void outputVal(FILE *fp, float val)
{
	wchar_t buf[1024];

	if ( val == 0.0f )
	{
		swprintf_s(buf, 1024, _T("\"0\""));
	}
	else
	{
		std::string str = toShortString(val);

		wchar_t	wstr[256];
		size_t wLen = 0;
		mbstowcs_s(&wLen, wstr, 256, str.c_str(), _TRUNCATE);

		swprintf_s(buf, 1024, _T("\"%s\""), wstr);
		//少数点以降が0の場合が整数にする
	}
	fputws(buf, fp);
}


/** 浮動小数点を短い文字列で返す */
std::string toShortString(float value)
{
	if (value == -0.0f) value = 0.0f;

	std::stringstream ss;
	double i;
	double d = std::modf(value, &i);

	// 小数部が0のときは小数１桁、それ以外は小数６桁にする。
	// 切り捨てられる小数部は四捨五入される
	int nd = (d == 0.0) ? 0 : 1;
	ss << std::setprecision(nd) << std::setiosflags(std::ios::fixed);

	ss << value;
	return ss.str();
}

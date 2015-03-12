// plistConverter.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

void gettexturesize(void);
void getcelldata(void);
void outputcelldata(void);

bool err = false;	//エラーが出たら処理中断
std::string open_filename;
std::string output_filename;
bool option_offset = true;

int tex_size_w = 0;	//テクスチャサイズ
int tex_size_h = 0;	//テクスチャサイズ

//セルデータ
struct CELLDATA
{
	int x;
	int y;
	int w;
	int h;
	int offset_x;
	int offset_y;
	std::string name;
};

#define SELLDATA_MAX 3000
CELLDATA celldata[SELLDATA_MAX];
int cell_count = 0;

int main(int argc, char* argv[])
{
	//引数チェック
	int i;
	bool ishelp = false;
	int inputcount = 0;

	if (argc == 1)
	{
		ishelp = true;
	}
	for (i = 1; i < argc; i++ )
	{
		if (
		     (strcmp(argv[i], "-h") == 0)
		  || (strcmp(argv[i], "-H") == 0)
		   )
		{
			//ヘルプ表示
			ishelp = true;
		}
		else if (
			   (strcmp(argv[i], "-o") == 0)
			|| (strcmp(argv[i], "-O") == 0)
			)
		{
			option_offset = false;
		}
		else
		{
			if (inputcount == 0)
			{
				open_filename = argv[i];
				inputcount++;
			}
			else
			{
				output_filename = argv[i];
			}
		}
	}
	//拡張子チェック
	if (open_filename.find(".plist") == std::string::npos)
	{
		err = true;	//エラーが出たら処理中断
	}

	if ( inputcount == 1 )
	{
		//出力ファイル名が省略されている場合
		output_filename = open_filename.substr(0, open_filename.length() - 5);
		output_filename = output_filename + "xml";
	}



	if (ishelp == true)
	{
		printf("第一引数：plistのパス\r\n");
		printf("第二引数：出力ファイルのパス（省略可）\r\n");
		printf("オプション\r\n");
		printf("-o：オフセット出力なし\r\n");
		printf("-h：ヘルプ表示\r\n");
	}
	else
	{
		if (err == false)
		{
			gettexturesize();	//テクスチャのサイズ取得
			getcelldata();		//セル情報の取得
			outputcelldata();	//セル情報の書き出し
		}
	}

	return 0;
}

//テクスチャサイズの取得
void gettexturesize(void)
{
	FILE *fp;
	char buf[1024];
	// ファイルのオープン
	fopen_s(&fp, open_filename.c_str(), "r");
	if (fp == NULL)
	{
		//エラー
		printf("file open error!!\n");
		err = true;
	}
	if ( err == false )
	{
		int seq = 0;
		int yomitobasi = 0;
		//１行単位で読み出し
		while (fgets(buf, 1024, fp) != NULL)
		{
			std::string str(buf);

			switch( seq )
			{
			case 0:
				//metadataタグを探す
				{
					unsigned int loc = str.find("metadata", 0);
					if (loc != std::string::npos)
					{
						//見つかった
						seq++;
						yomitobasi = 0;
					}
				}
				break;
			case 1:
				//6行読み飛ばす
				yomitobasi++;
				if (yomitobasi == 6 )
				{
					seq++;
				}
				break;
			case 2:
				//サイズを取得する
				unsigned int loc1 = str.find("{", 0);
				unsigned int loc2 = str.find(",", 0);
				unsigned int loc3 = str.find("}", 0);
				if (
				     (loc1 != std::string::npos)
			      && (loc2 != std::string::npos)
				  && (loc3 != std::string::npos)
				   )
				{
					//見つかった
					//数値を取得
					std::string w = str.substr(loc1+1, loc2 - loc1-1);
					std::string h = str.substr(loc2+1, loc3 - loc2-1);

					tex_size_w = std::atoi(w.c_str());;	//テクスチャサイズ
					tex_size_h = std::atoi(h.c_str());;	//テクスチャサイズ
				}
				break;
			}
		}
	}
	fclose(fp);	//ファイルのクローズ
}

//セルデータの取得
void getcelldata(void)
{
	FILE *fp;
	char buf[1024];
	// ファイルのオープン
	fopen_s(&fp, open_filename.c_str(), "r");
	if (fp == NULL)
	{
		//エラー
		printf("file open error!!\n");
		err = true;
	}
	if (err == false)
	{
		cell_count = 0;
		int seq = 0;
		int yomitobasi = 0;
		//１行単位で読み出し
		while (fgets(buf, 1024, fp) != NULL)
		{
			std::string str(buf);

			switch (seq)
			{
			case 0:
			//metadataタグを探す
			{
				unsigned int loc = str.find("<key>frame</key>", 0);
				if (loc != std::string::npos)
				{
					//見つかった
					seq++;
				}
				break;
			}
			case 1:
			{
				//サイズを取得する
				unsigned int loc1 = str.find("{{", 0);
				unsigned int loc2 = str.find(",", loc1 + 2);
				unsigned int loc3 = str.find("},{", loc2 + 1);
				unsigned int loc4 = str.find(",", loc3 + 3);
				unsigned int loc5 = str.find("}}", loc4 + 1);
				if (
						(loc1 != std::string::npos)
					&& (loc2 != std::string::npos)
					&& (loc3 != std::string::npos)
					&& (loc4 != std::string::npos)
					&& (loc5 != std::string::npos)
					)
				{
					//見つかった
					//数値を取得
					std::string x = str.substr(loc1 + 2, loc2 - loc1 - 2);
					std::string y = str.substr(loc2 + 1, loc3 - loc2 - 1);
					std::string w = str.substr(loc3 + 3, loc4 - loc3 - 3);
					std::string h = str.substr(loc4 + 1, loc5 - loc4 - 1);

					celldata[cell_count].x = std::atoi(x.c_str());;	//テクスチャサイズ
					celldata[cell_count].y = std::atoi(y.c_str());;	//テクスチャサイズ
					celldata[cell_count].w = std::atoi(w.c_str());;	//テクスチャサイズ
					celldata[cell_count].h = std::atoi(h.c_str());;	//テクスチャサイズ

					seq++;
				}
				break;
			}
			case 2:
			//オフセットを取得する
			{
				unsigned int loc = str.find("<key>offset</key>", 0);
				if (loc != std::string::npos)
				{
					//見つかった
					seq++;
				}
				break;
			}
			case 3:
			{
				//オフセットを取得する
				unsigned int loc1 = str.find("{", 0);
				unsigned int loc2 = str.find(",", loc1 + 1);
				unsigned int loc3 = str.find("}", loc2 + 1);
				if (
					   (loc1 != std::string::npos)
					&& (loc2 != std::string::npos)
					&& (loc3 != std::string::npos)
					)
				{
					//見つかった
					//数値を取得
					std::string x = str.substr(loc1 + 1, loc2 - loc1 - 1);
					std::string y = str.substr(loc2 + 1, loc3 - loc2 - 1);

					celldata[cell_count].offset_x = std::atoi(x.c_str());	//テクスチャサイズ
					celldata[cell_count].offset_y = std::atoi(y.c_str());	//テクスチャサイズ

					char name[128];
					sprintf_s(name, 128, "frame%03d.png", cell_count);
					celldata[cell_count].name = name;	//名前

					cell_count++;
					seq = 0;	//次のデータを作成する
				}
				break;
			}
			}
		}
	}
	fclose(fp);	//ファイルのクローズ
}

//セルデータの出力
void outputcelldata(void)
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
		fputws(_T("<root version=\"1.0\">\n"), fp);

		swprintf_s(buf, 1024, _T("  <info w=\"%d\" h=\"%d\" />\n"), tex_size_w, tex_size_h);
		fputws(buf, fp);
		fputws(_T("  <textures>\n"), fp);

		int i;
		for (i = 0; i < cell_count; i++ )
		{
			//変換
			wchar_t	name[50];
			size_t wLen = 0;
			mbstowcs_s(&wLen, name, 20, celldata[i].name.c_str(), _TRUNCATE);

			//セルデータの出力
			if (option_offset == false)
			{
				swprintf_s(buf, 1024, _T("    <texture filename=\"%s\" x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" />\n"), name, celldata[i].x, celldata[i].y, celldata[i].w, celldata[i].h);
			}
			else
			{
				swprintf_s(buf, 1024, _T("    <texture filename=\"%s\" x=\"%d\" y=\"%d\" w=\"%d\" h=\"%d\" offset_x=\"%d\" offset_y=\"%d\" />\n"), name, celldata[i].x, celldata[i].y, celldata[i].w, celldata[i].h, celldata[i].offset_x, celldata[i].offset_y);
			}

			fputws(buf, fp);
		}
		//フッダ出力
		fputws(_T("  </textures>\n"), fp);
		fputws(_T("</root>\n"), fp);
		fputws(_T("\n"), fp);

	}
	fclose(fp);	//ファイルのクローズ
}

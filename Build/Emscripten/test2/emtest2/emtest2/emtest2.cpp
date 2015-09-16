// emtest2.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

//#include "stdafx.h"
#include "tinyxml2.h"
#include <emscripten.h>

//JS側から与えられたXMLファイル名を受け取Ｃ側のＸＭＬパーサーが使用可能かどうかをチェックするプログラム
extern "C" {
	int MergeFile(const char* x);
};

//メインループ
void one_iter() {
	static int status = 0;
	using namespace tinyxml2;

	switch(status)
	{
	case 0://project file 追加待ちを監視
		{
			XMLDocument		xml;
			if ( XML_SUCCESS == xml.LoadFile("input.sspj") )
			{
				printf( "XML_SUCCESS" );
				xml.Print();
				status = 1;
			}else{
				printf( "XML_ERROR_FILE_NOT_FOUND" );
			}
		}
		break;
	case 1:
		{

		}
		break;
	}
}

int main(void)
{
	//使用するファイルの追加
	MergeFile( "input.sspj" );

	emscripten_set_main_loop(one_iter, 16,1);

	return 0;
}


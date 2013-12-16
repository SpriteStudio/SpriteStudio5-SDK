//
//	Windows / OSX　共通で呼び出せるファイル選択ダイアログライブラリ
//
//
//  XPFileOpenDlg.cpp
//  sssdk
//
//  Created by kurooka on 2013/07/18.
//
//
#include "MacOS/OSXFileOpen.h"
#include "XPFileOpenDlg.h"

#ifdef _WIN32
	//コモンダイアログを使うため
	#include <windows.h>
	#include <tchar.h>
	#include <stdlib.h>
	#include <stdio.h>
#endif

#include "string"

bool XPFileOpenDlg::Show()
{	
#ifdef _WIN32
	static OPENFILENAME ofn;
	static TCHAR filename_full[MAX_PATH];   // ファイル名(フルパス)を受け取る領域
	static TCHAR filename[MAX_PATH];        // ファイル名を受け取る領域

	// 構造体に情報をセット
	ZeroMemory( &ofn, sizeof(ofn) );       // 最初にゼロクリアしておく
	ofn.lStructSize = sizeof(ofn);         // 構造体のサイズ
	ofn.hwndOwner = 0;                  // コモンダイアログの親ウィンドウハンドル
	ofn.lpstrFilter = _T("sspj(*.sspj)\0*.sspj\0All files(*.*)\0*.*\0\0"); // ファイルの種類
	ofn.lpstrFile = filename_full;         // 選択されたファイル名(フルパス)を受け取る変数のアドレス
	ofn.lpstrFileTitle = filename;         // 選択されたファイル名を受け取る変数のアドレス
	ofn.nMaxFile = sizeof(filename_full);  // lpstrFileに指定した変数のサイズ
	ofn.nMaxFileTitle = sizeof(filename);  // lpstrFileTitleに指定した変数のサイズ
	ofn.Flags = OFN_FILEMUSTEXIST;         // フラグ指定
	ofn.lpstrTitle = _T("ファイルを開く"); // コモンダイアログのキャプション
	ofn.lpstrDefExt = _T("sspj");           // デフォルトのファイルの種類

	// ファイルを開くコモンダイアログを作成
	if( !GetOpenFileName( &ofn ) )
	{
	}else{
		//マルチバイト変換　(ユニコードの場合失敗する)
		size_t	nLen;

		nLen = ::_tcslen(filename_full) + 1;
		pszChar = new char[nLen];
		if(pszChar)
			::strcpy_s(pszChar,nLen * sizeof(char),filename_full);

		return true;
	}

#else
    MacOSXFileOpenDlg dlg;
    if ( dlg.show() )
    {
        //pszCharへの実装をする
        pszChar = new char[2048];
        strcpy( pszChar , dlg.getFilePath() );
        return true;
    }
#endif

    return false;
}
